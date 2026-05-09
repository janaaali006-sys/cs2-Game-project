#ifndef LOGICRIDDLE_H
#define LOGICRIDDLE_H

#include "Riddle.h"

class LogicRiddle : public Riddle {
public:
    LogicRiddle(QString prompt, QString answer, QString hint, QString godName, Difficulty difficulty);

    QString getPrompt() const override;
    QString getAnswer() const override;
    QString getHint() const override;
    RiddleType getType() const override;
};

#endif
