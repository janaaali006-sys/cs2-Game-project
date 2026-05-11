#ifndef RIDDLEGENERATOR_H
#define RIDDLEGENERATOR_H

#include <QNetworkAccessManager>
#include "AIGenerator.h"
#include "SharedEnums.h"

class Riddle;
class IdentityRiddle;
class LogicRiddle;
class SequenceRiddle;

// ============================================================
// RiddleGenerator — inherits AIGenerator.
//
// OOP role: Inheritance + Polymorphism
//   - Extends AIGenerator with riddle-specific generation.
//   - Overrides setApiKey() (pure virtual in AIGenerator).
//   - Returns Riddle* (abstract type) — caller gets back an
//     IdentityRiddle, LogicRiddle, or SequenceRiddle via the
//     base pointer (polymorphism in action).
//   - Falls back to RiddleFallbackPool if the API fails.
// ============================================================
class RiddleGenerator : public AIGenerator {
    Q_OBJECT

public:
    explicit RiddleGenerator(QObject* parent = nullptr);

    // Implement pure virtual from AIGenerator
    void setApiKey(const QString& key) override { m_apiKey = key; }

    // Main entry point — returns a Riddle* (concrete subtype decided internally)
    Riddle* generate(const QString& godName,
                     Difficulty     difficulty,
                     RiddleType     type,
                     RoomMood       mood  = RoomMood::CALM,
                     bool           useAI = true);

private:
    QNetworkAccessManager m_network;

    QString buildPrompt(const QString& godName,
                        Difficulty     difficulty,
                        RiddleType     type,
                        RoomMood       mood) const;

    Riddle* callClaudeAPI(const QString& godName,
                          Difficulty     difficulty,
                          RiddleType     type,
                          RoomMood       mood);

    Riddle* parseJsonResponse(const QString& json,
                              RiddleType     expectedType,
                              const QString& godName,
                              Difficulty     difficulty);

    Riddle* getFallback(const QString& godName, RiddleType type);
};

#endif // RIDDLEGENERATOR_H
