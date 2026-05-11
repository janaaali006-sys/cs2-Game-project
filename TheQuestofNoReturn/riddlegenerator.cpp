#include "riddlegenerator.h"
#include "riddles/Riddle.h"
#include "riddles/IdentityRiddle.h"
#include "riddles/LogicRiddle.h"
#include "riddles/SequenceRiddle.h"
#include "riddles/RiddleFallbackPool.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEventLoop>
#include <QTimer>
#include <QRandomGenerator>
#include <QDebug>

RiddleGenerator::RiddleGenerator(QObject* parent) : AIGenerator(parent) {}

// ============================================================
// Main entry point
// ============================================================

Riddle* RiddleGenerator::generate(const QString& godName,
                                   Difficulty difficulty,
                                   RiddleType type,
                                   RoomMood   mood,
                                   bool       useAI) {
    if (useAI && m_useAI && !m_apiKey.isEmpty()) {
        Riddle* result = callGeminiAPI(godName, difficulty, type, mood);
        if (result) return result;
        qDebug() << "Gemini API failed — using fallback riddle";
    }
    return getFallback(godName, type);
}

// ============================================================
// Build the prompt
// ============================================================

QString RiddleGenerator::buildPrompt(const QString& godName,
                                      Difficulty difficulty,
                                      RiddleType type,
                                      RoomMood   mood) const {
    QString diffStr = (difficulty == Difficulty::EASY)   ? "easy" :
                      (difficulty == Difficulty::MEDIUM) ? "medium" : "hard";

    QString typeStr = (type == RiddleType::IDENTITY) ? "identity (Who am I? format)" :
                      (type == RiddleType::LOGIC)    ? "logic (reasoning/deduction)" :
                                                       "sequence (pattern completion)";

    QString cursedExtra = (mood == RoomMood::CURSED)
        ? " Also add a field \"decoy\" with one plausible but incorrect answer."
        : "";

    QString hardExtra = (difficulty == Difficulty::HARD)
        ? " Include one subtle misdirection in the riddle clues."
        : "";

    return QString(
        "Generate a %1 difficulty %2 riddle for an Egyptian temple adventure game. "
        "The riddle must relate to the Egyptian god or figure: %3. "
        "Return ONLY a valid JSON object with no extra text, no markdown, no backticks. "
        "Format exactly: "
        "{\"question\":\"the full riddle text\","
        "\"answer\":\"single word or short phrase\","
        "\"hint\":\"a helpful hint without giving away the answer\"}"
        "%4%5"
    ).arg(diffStr, typeStr, godName, hardExtra, cursedExtra);
}

// ============================================================
// Call Gemini API (free tier — gemini-2.0-flash model)
// ============================================================

Riddle* RiddleGenerator::callGeminiAPI(const QString& godName,
                                        Difficulty difficulty,
                                        RiddleType type,
                                        RoomMood   mood) {
    // Gemini endpoint — key goes in URL as query param (no auth header needed)
    QString url = QString("https://generativelanguage.googleapis.com/v1beta/models/"
                          "gemini-2.0-flash:generateContent?key=%1").arg(m_apiKey);

    // Gemini request body format
    QJsonObject textPart;
    textPart["text"] = buildPrompt(godName, difficulty, type, mood);

    QJsonObject content;
    content["parts"] = QJsonArray{ textPart };

    QJsonObject body;
    body["contents"] = QJsonArray{ content };

    // Generation config — keep response short and deterministic
    QJsonObject genConfig;
    genConfig["temperature"]     = 0.7;
    genConfig["maxOutputTokens"] = 300;
    body["generationConfig"]     = genConfig;

    QNetworkRequest request{QUrl(url)};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_network.post(request, QJsonDocument(body).toJson());

    // Wait up to 8 seconds
    QEventLoop loop;
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    timeoutTimer.setInterval(8000);
    connect(reply,         &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timeoutTimer, &QTimer::timeout,          &loop, &QEventLoop::quit);
    timeoutTimer.start();
    loop.exec();

    if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
        qDebug() << "Gemini API error:" << reply->errorString();
        reply->deleteLater();
        return nullptr;
    }

    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    // Gemini response structure:
    // { "candidates": [ { "content": { "parts": [ { "text": "..." } ] } } ] }
    QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);
    if (responseDoc.isNull()) return nullptr;

    QJsonArray candidates = responseDoc.object()["candidates"].toArray();
    if (candidates.isEmpty()) {
        qDebug() << "Gemini returned no candidates:" << responseData;
        return nullptr;
    }

    QString text = candidates[0].toObject()["content"]
                                .toObject()["parts"]
                                .toArray()[0]
                                .toObject()["text"]
                                .toString()
                                .trimmed();

    return parseJsonResponse(text, type, godName, difficulty);
}

// ============================================================
// Parse JSON response into typed Riddle subclass
// ============================================================

Riddle* RiddleGenerator::parseJsonResponse(const QString& json,
                                            RiddleType expectedType,
                                            const QString& godName,
                                            Difficulty difficulty) {
    QString clean = json;
    clean = clean.replace("```json", "").replace("```", "").trimmed();

    QJsonDocument doc = QJsonDocument::fromJson(clean.toUtf8());
    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "Failed to parse riddle JSON:" << clean;
        return nullptr;
    }

    QJsonObject obj = doc.object();
    QString question = obj["question"].toString().trimmed();
    QString answer   = obj["answer"].toString().trimmed();
    QString hint     = obj["hint"].toString().trimmed();

    if (question.isEmpty() || answer.isEmpty()) return nullptr;

    Riddle* riddle = nullptr;
    switch (expectedType) {
    case RiddleType::IDENTITY:
        riddle = new IdentityRiddle(question, answer, hint, godName, difficulty);
        break;
    case RiddleType::LOGIC:
        riddle = new LogicRiddle(question, answer, hint, godName, difficulty);
        break;
    case RiddleType::SEQUENCE:
        riddle = new SequenceRiddle(question, answer, hint, godName, difficulty);
        break;
    }
    return riddle;
}

// ============================================================
// Fallback
// ============================================================

Riddle* RiddleGenerator::getFallback(const QString& godName, RiddleType type) {
    static RiddleFallbackPool pool;
    Riddle* r = pool.getRandom(type, godName);
    if (r) qDebug() << "Using fallback riddle for type" << (int)type;
    return r;
}
