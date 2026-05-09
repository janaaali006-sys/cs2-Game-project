#include "riddlegenerator.h"
#include "riddle.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>

RiddleGenerator::RiddleGenerator(QObject* parent) : QObject(parent) {}

// Main entry point

Riddle* RiddleGenerator::generate(const QString& godName,
                                  Difficulty difficulty,
                                  RiddleType type,
                                  bool useAI) {
    if (useAI && m_useAI && !m_apiKey.isEmpty()) {
        Riddle* result = callClaudeAPI(godName, difficulty, type);
        if (result) return result;
        // if API fails → fall through to fallback
        qDebug() << "Claude API failed, using fallback riddle";
    }
    return getFallback(godName, type);
}

// Build the Claude prompt

QString RiddleGenerator::buildPrompt(const QString& godName,
                                     Difficulty difficulty,
                                     RiddleType type) const {
    QString diffStr = (difficulty == Difficulty::EASY)   ? "easy" :
                          (difficulty == Difficulty::MEDIUM) ? "medium" : "hard";

    QString typeStr = (type == RiddleType::IDENTITY) ? "identity (Who am I? format)" :
                          (type == RiddleType::LOGIC)    ? "logic (reasoning/deduction)" :
                          "sequence (pattern completion)";

    QString hardExtra = (difficulty == Difficulty::HARD)
                            ? " Include one subtle misdirection in the clues."
                            : "";

    return QString(
               "Generate a %1 difficulty %2 riddle for an Egyptian temple adventure game. "
               "The riddle must relate to the Egyptian god or figure: %3. "
               "Return ONLY a valid JSON object with no extra text, no markdown, no backticks. "
               "Format exactly: "
               "{\"question\":\"the full riddle text here\","
               "\"answer\":\"single word or short phrase answer\","
               "\"hint\":\"a helpful hint without giving away the answer\"}"
               "%4"
               ).arg(diffStr, typeStr, godName, hardExtra);
}

// Call Claude API (synchronous via QEventLoop)

Riddle* RiddleGenerator::callClaudeAPI(const QString& godName,
                                       Difficulty difficulty,
                                       RiddleType type) {
    // Build request body
    QJsonObject message;
    message["role"]    = "user";
    message["content"] = buildPrompt(godName, difficulty, type);

    QJsonObject body;
    body["model"]      = "claude-sonnet-4-20250514";
    body["max_tokens"] = 300;
    body["messages"]   = QJsonArray{ message };

    QNetworkRequest request(QUrl("https://api.anthropic.com/v1/messages"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("x-api-key", m_apiKey.toUtf8());
    request.setRawHeader("anthropic-version", "2023-06-01");

    // Send request
    QNetworkReply* reply = m_network.post(request, QJsonDocument(body).toJson());

    // Wait max 8 seconds for response
    QEventLoop loop;
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    timeoutTimer.setInterval(8000);

    connect(reply,        &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timeoutTimer, &QTimer::timeout,         &loop, &QEventLoop::quit);
    timeoutTimer.start();
    loop.exec();

    if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
        qDebug() << "Claude API error:" << reply->errorString();
        reply->deleteLater();
        return nullptr;
    }

    // Parse response
    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);
    if (responseDoc.isNull()) return nullptr;

    // Claude response structure:
    QJsonArray content = responseDoc.object()["content"].toArray();
    if (content.isEmpty()) return nullptr;

    QString text = content[0].toObject()["text"].toString().trimmed();
    return parseJsonResponse(text, type, godName, difficulty);
}

// Parse JSON from Claude into a Riddle*

Riddle* RiddleGenerator::parseJsonResponse(const QString& json,
                                           RiddleType expectedType,
                                           const QString& godName,
                                           Difficulty difficulty) {
    // Strip any accidental markdown backticks
    QString clean = json;
    clean.remove("```json").remove("```").remove("```").trimmed();

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
    case RiddleType::IDENTITY: riddle = new IdentityRiddle(question, answer, hint); break;
    case RiddleType::LOGIC:    riddle = new LogicRiddle(question, answer, hint);    break;
    case RiddleType::SEQUENCE: riddle = new SequenceRiddle(question, answer, hint); break;
    }

    if (riddle) {
        riddle->setGodName(godName);
        riddle->setDifficulty(difficulty);
    }
    return riddle;
}

// Fallback riddle pool
// Used when API is down or useAI = false
// These cover all 6 gods across all 3 types

Riddle* RiddleGenerator::getFallback(const QString& godName, RiddleType type) {
    // Indexed by: god + type → returns a hardcoded riddle
    // Each god has 3 riddles (one per type)
    // We pick randomly among all for that type

    struct RiddleData { QString q, a, h; };

    QVector<RiddleData> identityPool = {
                                        {"I have the head of a hawk and the eye that sees all. I protect the pharaoh and battle Set. Who am I?",
                                         "Horus", "Son of Osiris and Isis"},
                                        {"I weigh the heart against a feather. I guide the dead and am the god of embalming. Who am I?",
                                         "Anubis", "I have the head of a jackal"},
                                        {"I travel across the sky in a golden boat, bringing light to the living and darkness to the dead. Who am I?",
                                         "Ra", "The sun god"},
                                        {"I have the body of a lion and the head of a human. I guard the pyramid and ask riddles of travellers. Who am I?",
                                         "Sphinx", "Half human, half lion"},
                                        {"I rule the afterlife and am shown holding a crook and flail. I was killed by Set and resurrected by Isis. Who am I?",
                                         "Osiris", "God of the dead and resurrection"},
                                        {"I am the symbol of life itself. I am carried by gods and pharaohs. I am shaped like a cross with a loop. What am I?",
                                         "Ankh", "The key of life"},
                                        };

    QVector<RiddleData> logicPool = {
                                     {"Three doors stand before you. The first door has a sun painted on it. The second has a moon. The third has a star. Ra said: I am not the moon, and I am not the star. Behind which door does Ra hide?",
                                      "The first door", "Ra is the sun god"},
                                     {"Anubis weighs two hearts. Heart A weighs the same as the feather. Heart B is heavier than Heart A. Which heart goes to the Field of Reeds?",
                                      "Heart A", "The scales must balance with the feather of Ma'at"},
                                     {"A tomb has four chambers. The gold is not in the first chamber. The scarab is not in the last. The papyrus is next to the gold. The canopic jar is in the third chamber. Where is the gold?",
                                      "Second chamber", "The papyrus must be next to it"},
                                     {"If Horus flies east at dawn and Set flies west at dusk, and they both fly for 4 hours at the same speed, who is further from the temple at midnight?",
                                      "They are the same distance", "They fly equal time and speed"},
                                     {"The pharaoh has 5 advisors. Two worship Ra, two worship Osiris, one worships Anubis. If the Anubis worshipper always lies, and the Ra worshippers always tell truth, and the Anubis worshipper says 'I worship Ra' — what does this tell us?",
                                      "Nothing, because liars claim what is false", "What does a liar say about themselves?"},
                                     {"In the Hall of Ra, there are 12 columns. Every 3rd column has a torch. Every 4th column has a hieroglyph. How many columns have both?",
                                      "One (column 12)", "Find the lowest common multiple of 3 and 4"},
                                     };

    QVector<RiddleData> sequencePool = {
                                        {"Ra's daily journey: Dawn → Noon → Dusk → ___. What comes next?",
                                         "The Duat (underworld)", "Where does the sun go at night?"},
                                        {"The mummification steps: Removal of organs → Drying with natron → Wrapping in linen → ___. What is the final step?",
                                         "Placement in the sarcophagus", "Where does the mummy rest?"},
                                        {"The sequence of Egyptian crowns: White → Red → Double → ___. What crown comes next in the cycle?",
                                         "Blue (Khepresh)", "The war crown of the pharaoh"},
                                        {"The flood cycle of the Nile: Inundation → Recession → ___. What is the third season?",
                                         "Harvest", "When does Egypt plant and reap?"},
                                        {"Osiris was: Alive → Killed by Set → Resurrected by Isis → ___. What is his final state?",
                                         "Ruler of the afterlife", "Where did Osiris go after resurrection?"},
                                        {"The plagues sequence begins: Water to blood → Frogs → Lice → Flies → ___. What is the fifth plague?",
                                         "Livestock disease", "It strikes the animals of Egypt"},
                                        };

    // Select pool based on type
    QVector<RiddleData>* pool = nullptr;
    switch (type) {
    case RiddleType::IDENTITY: pool = &identityPool; break;
    case RiddleType::LOGIC:    pool = &logicPool;    break;
    case RiddleType::SEQUENCE: pool = &sequencePool; break;
    }

    if (!pool || pool->isEmpty()) return nullptr;

    // Pick a random one from the pool
    int idx = QRandomGenerator::global()->bounded(pool->size());
    auto& data = (*pool)[idx];

    Riddle* riddle = nullptr;
    switch (type) {
    case RiddleType::IDENTITY: riddle = new IdentityRiddle(data.q, data.a, data.h); break;
    case RiddleType::LOGIC:    riddle = new LogicRiddle(data.q, data.a, data.h);    break;
    case RiddleType::SEQUENCE: riddle = new SequenceRiddle(data.q, data.a, data.h); break;
    }
    if (riddle) riddle->setGodName(godName);
    return riddle;
}
