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
        Riddle* result = callClaudeAPI(godName, difficulty, type, mood);
        if (result) return result;
        qDebug() << "Claude API failed — using fallback riddle";
    }
    return getFallback(godName, type);
}

// ============================================================
// Build the prompt — CURSED rooms add a decoy instruction
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

    // CURSED room: ask Claude to include a plausible wrong decoy answer
    QString cursedExtra = (mood == RoomMood::CURSED)
        ? " Also add a field \\\"decoy\\\" with one plausible but incorrect answer to mislead the player."
        : "";

    // HARD difficulty: add misdirection in the clues themselves
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
// Call Claude API (synchronous via QEventLoop, 8s timeout)
// ============================================================

Riddle* RiddleGenerator::callClaudeAPI(const QString& godName,
                                        Difficulty difficulty,
                                        RiddleType type,
                                        RoomMood   mood) {
    QJsonObject message;
    message["role"]    = "user";
    message["content"] = buildPrompt(godName, difficulty, type, mood);

    QJsonObject body;
    body["model"]      = "claude-sonnet-4-20250514";
    body["max_tokens"] = 300;
    body["messages"]   = QJsonArray{ message };

    QNetworkRequest request(QUrl("https://api.anthropic.com/v1/messages"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("x-api-key",          m_apiKey.toUtf8());
    request.setRawHeader("anthropic-version",  "2023-06-01");

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
        qDebug() << "Claude API error:" << reply->errorString();
        reply->deleteLater();
        return nullptr;
    }

    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);
    if (responseDoc.isNull()) return nullptr;

    QJsonArray content = responseDoc.object()["content"].toArray();
    if (content.isEmpty()) return nullptr;

    QString text = content[0].toObject()["text"].toString().trimmed();
    return parseJsonResponse(text, type, godName, difficulty);
}

// ============================================================
// Parse Claude's JSON into a typed Riddle subclass
// ============================================================

Riddle* RiddleGenerator::parseJsonResponse(const QString& json,
                                            RiddleType expectedType,
                                            const QString& godName,
                                            Difficulty difficulty) {
    QString clean = json;
    clean.remove("```json").remove("```").trimmed();

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

    // Use the correct Riddle subclass (polymorphism via Riddle* base pointer)
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
// Fallback — delegates to RiddleFallbackPool (Person 2's class)
// ============================================================

Riddle* RiddleGenerator::getFallback(const QString& godName, RiddleType type) {
    static RiddleFallbackPool pool;   // built once, reused
    Riddle* r = pool.getRandom(type, godName);
    if (r) qDebug() << "Using fallback riddle for type" << (int)type;
    return r;
}
