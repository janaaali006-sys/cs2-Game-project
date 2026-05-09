#include "avatargenerator.h"

#include <QFile>
#include <QFileInfo>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QEventLoop>
#include <QDebug>

AvatarGenerator::AvatarGenerator(QObject* parent) : QObject(parent) {
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setSingleShot(true);
    m_timeoutTimer->setInterval(15000);
    connect(m_timeoutTimer, &QTimer::timeout, this, &AvatarGenerator::onTimeout);
}

void AvatarGenerator::onTimeout() {
    qDebug() << "Avatar generation timed out";
    emit generationFailed();
}

void AvatarGenerator::generateFromPhoto(const QString& photoPath) {
    m_photoPath = photoPath;
    m_timeoutTimer->start();
    emit progressUpdate("Reading your photo...");

    // Read photo file
    QFile file(photoPath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit generationFailed();
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    // Encode to Base64
    QString base64 = fileData.toBase64();

    // Determine MIME type from extension
    QString ext = QFileInfo(photoPath).suffix().toLower();
    QString mimeType = (ext == "png") ? "image/png" : "image/jpeg";

    emit progressUpdate("Analysing your appearance with AI...");
    stepOneDescribe(base64, mimeType);
}

// Step 1: Send photo to Claude, get appearance description

void AvatarGenerator::stepOneDescribe(const QString& base64Image,
                                      const QString& mimeType) {
    // Build message with image content
    QJsonObject imageSource;
    imageSource["type"]       = "base64";
    imageSource["media_type"] = mimeType;
    imageSource["data"]       = base64Image;

    QJsonObject imageContent;
    imageContent["type"]   = "image";
    imageContent["source"] = imageSource;

    QJsonObject textContent;
    textContent["type"] = "text";
    textContent["text"] = "Describe this person's physical appearance for use in an illustration prompt. "
                          "Include: hair color and style, skin tone, eye color, face shape, approximate age range. "
                          "Be specific but concise — 2-3 sentences only. Do not mention clothing in detail.";

    QJsonObject message;
    message["role"]    = "user";
    message["content"] = QJsonArray{ imageContent, textContent };

    QJsonObject body;
    body["model"]      = "claude-sonnet-4-20250514";
    body["max_tokens"] = 200;
    body["messages"]   = QJsonArray{ message };

    QNetworkRequest request(QUrl("https://api.anthropic.com/v1/messages"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("x-api-key", m_claudeKey.toUtf8());
    request.setRawHeader("anthropic-version", "2023-06-01");

    QNetworkReply* reply = m_network.post(request, QJsonDocument(body).toJson());

    // Wait for reply (with our outer timeout still running)
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Claude vision error:" << reply->errorString();
        reply->deleteLater();
        emit generationFailed();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QString description = doc.object()["content"]
                              .toArray()[0]
                              .toObject()["text"]
                              .toString()
                              .trimmed();

    if (description.isEmpty()) {
        emit generationFailed();
        return;
    }

    qDebug() << "Got appearance description:" << description;
    emit progressUpdate("Generating your Egyptian explorer portrait...");
    stepTwoRender(description);
}

// Step 2: Send description to DALL-E 3, get image URL

void AvatarGenerator::stepTwoRender(const QString& description) {
    QString prompt = QString(
                         "Egyptian archaeologist explorer portrait, %1, "
                         "wearing archaeological field gear and Egyptian amulets, "
                         "cinematic game concept art style, dramatic torch-lit ancient temple background, "
                         "dark atmospheric lighting, highly detailed face, portrait orientation, "
                         "professional digital painting"
                         ).arg(description);

    QJsonObject body;
    body["model"]   = "dall-e-3";
    body["prompt"]  = prompt;
    body["n"]       = 1;
    body["size"]    = "1024x1024";
    body["quality"] = "standard";

    QNetworkRequest request(QUrl("https://api.openai.com/v1/images/generations"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization",
                         QString("Bearer %1").arg(m_openAiKey).toUtf8());

    QNetworkReply* reply = m_network.post(request, QJsonDocument(body).toJson());

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "DALL-E error:" << reply->errorString();
        reply->deleteLater();
        emit generationFailed();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QString imageUrl = doc.object()["data"]
                           .toArray()[0]
                           .toObject()["url"]
                           .toString();

    if (imageUrl.isEmpty()) {
        emit generationFailed();
        return;
    }

    emit progressUpdate("Downloading your portrait...");
    stepThreeDownload(imageUrl);
}

// Step 3: Download image, save locally, emit avatarReady

void AvatarGenerator::stepThreeDownload(const QString& imageUrl) {
    QNetworkRequest request(QUrl(imageUrl));
    QNetworkReply* reply = m_network.get(request);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        emit generationFailed();
        return;
    }

    QByteArray imageData = reply->readAll();
    reply->deleteLater();

    // Save to temp location
    QString savePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                       + "/quest_avatar.png";

    QFile outFile(savePath);
    if (!outFile.open(QIODevice::WriteOnly)) {
        emit generationFailed();
        return;
    }
    outFile.write(imageData);
    outFile.close();

    m_timeoutTimer->stop();
    qDebug() << "Avatar saved to:" << savePath;
    emit avatarReady(savePath);
}
