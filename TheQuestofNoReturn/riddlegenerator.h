#ifndef RIDDLEGENERATOR_H
#define RIDDLEGENERATOR_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include "SharedEnums.h"

class Riddle;
class IdentityRiddle;
class LogicRiddle;
class SequenceRiddle;

class RiddleGenerator : public QObject {
    Q_OBJECT

public:
    explicit RiddleGenerator(QObject* parent = nullptr);

    Riddle* generate(const QString& godName,
                     Difficulty     difficulty,
                     RiddleType     type,
                     bool           useAI = true);

    void setUseAI(bool val) { m_useAI = val; }
    bool getUseAI()   const { return m_useAI; }

    void setApiKey(const QString& key) { m_apiKey = key; }

private:
    bool                  m_useAI  = true;
    QString               m_apiKey = "";
    QNetworkAccessManager m_network;

    QString buildPrompt(const QString& godName,
                        Difficulty     difficulty,
                        RiddleType     type) const;

    Riddle* callClaudeAPI(const QString& godName,
                          Difficulty     difficulty,
                          RiddleType     type);

    Riddle* parseJsonResponse(const QString& json,
                              RiddleType     expectedType,
                              const QString& godName,
                              Difficulty     difficulty);

    Riddle* getFallback(const QString& godName, RiddleType type);
};
