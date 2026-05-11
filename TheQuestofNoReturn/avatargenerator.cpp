#include "avatargenerator.h"
#include "AIGenerator.h"

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

AvatarGenerator::AvatarGenerator(QObject* parent) : AIGenerator(parent) {
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setSingleShot(true);
    m_timeoutTimer->setInterval(30000);  // 30s — HF can be slow
    connect(m_timeoutTimer, &QTimer::timeout, this, &AvatarGenerator::onTimeout);
}

void AvatarGenerator::useLocalPhotoFallback(const QString& reason)
{
    qDebug() << "Avatar fallback to local photo:" << reason;
    m_timeoutTimer->stop();
    emit progressUpdate("AI avatar unavailable (" + reason + ") - using your uploaded photo.");
    if (!m_photoPath.isEmpty()) {
        emit avatarReady(m_photoPath);
    } else {
        emit generationFailed();
    }
}

void AvatarGenerator::onTimeout() {
    useLocalPhotoFallback("Request timed out");
}

void AvatarGenerator::generateFromPhoto(const QString& photoPath) {
    m_photoPath = photoPath;
    m_timeoutTimer->start();
    emit progressUpdate("Reading your photo...");

    QFile file(photoPath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit generationFailed();
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QString base64 = fileData.toBase64();
    QString ext = QFileInfo(photoPath).suffix().toLower();
    QString mimeType = (ext == "png") ? "image/png" : "image/jpeg";

    if (m_apiKey.trimmed().isEmpty() || m_openAiKey.trimmed().isEmpty()) {
        useLocalPhotoFallback("Missing API key(s)");
        return;
    }

    emit progressUpdate("Analysing your appearance with AI...");
    stepOneDescribe(base64, mimeType);
}

// Step 1: Send photo to Gemini Vision, get appearance description

void AvatarGenerator::stepOneDescribe(const QString& base64Image,
                                      const QString& mimeType) {
    // Use Gemini for vision (free) — m_apiKey holds the Gemini key
    QString url = QString("https://generativelanguage.googleapis.com/v1beta/models/"
                          "gemini-2.0-flash:generateContent?key=%1").arg(m_apiKey);

    QJsonObject imagePart;
    QJsonObject inlineData;
    inlineData["mime_type"] = mimeType;
    inlineData["data"]      = base64Image;
    imagePart["inline_data"] = inlineData;

    QJsonObject textPart;
    textPart["text"] = "Describe this person's physical appearance for an illustration prompt. "
                       "Include: hair color and style, skin tone, eye color, face shape, approximate age range. "
                       "Be specific but concise — 2-3 sentences only.";

    QJsonObject content;
    content["parts"] = QJsonArray{ imagePart, textPart };

    QJsonObject body;
    body["contents"] = QJsonArray{ content };

    QNetworkRequest request{QUrl(url)};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_network.post(request, QJsonDocument(body).toJson());

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Gemini vision error:" << reply->errorString();
        reply->deleteLater();
        useLocalPhotoFallback("Gemini vision request failed");
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QString description = doc.object()["candidates"]
                              .toArray()[0]
                              .toObject()["content"]
                              .toObject()["parts"]
                              .toArray()[0]
                              .toObject()["text"]
                              .toString()
                              .trimmed();

    if (description.isEmpty()) {
        useLocalPhotoFallback("Gemini returned empty description");
        return;
    }

    qDebug() << "Got appearance description:" << description;
    emit progressUpdate("Generating your Egyptian explorer portrait...");
    stepTwoRender(description);
}

// Step 2: Send description to Hugging Face FLUX model, get image bytes

void AvatarGenerator::stepTwoRender(const QString& description) {
    QString prompt = QString(
        "Egyptian archaeologist explorer portrait, %1, "
        "wearing archaeological field gear and Egyptian amulets, "
        "cinematic game concept art style, dramatic torch-lit ancient temple background, "
        "dark atmospheric lighting, highly detailed face, portrait orientation"
    ).arg(description);

    // Hugging Face Inference API — FLUX.1-schnell is free
    QJsonObject body;
    body["inputs"] = prompt;

    // m_openAiKey holds the HuggingFace token
    QNetworkRequest request{QUrl("https://router.huggingface.co/hf-inference/models/black-forest-labs/FLUX.1-schnell/v1/images/generations")};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization",
                         QString("Bearer %1").arg(m_openAiKey).toUtf8());

    QNetworkReply* reply = m_network.post(request, QJsonDocument(body).toJson());

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "HuggingFace error:" << reply->errorString();
        reply->deleteLater();
        useLocalPhotoFallback("Image generation request failed");
        return;
    }

    // HF returns raw image bytes directly (not JSON)
    QByteArray imageData = reply->readAll();
    reply->deleteLater();

    if (imageData.isEmpty()) {
        useLocalPhotoFallback("Image generation returned empty data");
        return;
    }

    emit progressUpdate("Saving your portrait...");
    stepThreeDownload(imageData);
}

// Step 3: Save image bytes to disk, emit avatarReady

void AvatarGenerator::stepThreeDownload(const QByteArray& imageData) {
    QString savePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                       + "/quest_avatar.png";

    QFile outFile(savePath);
    if (!outFile.open(QIODevice::WriteOnly)) {
        useLocalPhotoFallback("Could not save generated image");
        return;
    }
    outFile.write(imageData);
    outFile.close();

    m_timeoutTimer->stop();
    qDebug() << "Avatar saved to:" << savePath;
    emit avatarReady(savePath);
}
