#ifndef RIDDLE_H
#define RIDDLE_H

#include <QString>
#include "../SharedEnums.h"

class Riddle {
public:
    Riddle(QString prompt, QString answer, QString hint, RiddleType type, QString godName, Difficulty difficulty);
    virtual ~Riddle() {}

    virtual QString getPrompt() const = 0;
    virtual QString getAnswer() const = 0;
    virtual QString getHint() const = 0;
    virtual RiddleType getType() const = 0;

    bool checkAnswer(QString input) const;

    QString getGodName() const;
    Difficulty getDifficulty() const;

protected:
    QString m_prompt;
    QString m_answer;
    QString m_hint;
    QString m_godName;
    Difficulty m_difficulty;
};

#endif
