#ifndef RIDDLEGENERATOR_H
#define RIDDLEGENERATOR_H

#include <QNetworkAccessManager>
#include "AIGenerator.h"
#include "SharedEnums.h"

class Riddle;
class IdentityRiddle;
class LogicRiddle;
class SequenceRiddle;

class RiddleGenerator : public AIGenerator {
    Q_OBJECT

public:
    explicit RiddleGenerator(QObject* parent = nullptr);

    void setApiKey(const QString& key) override { m_apiKey = key; }

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

    // Now calls Gemini instead of Claude
    Riddle* callGeminiAPI(const QString& godName,
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
