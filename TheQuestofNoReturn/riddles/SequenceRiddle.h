#ifndef SEQUENCERIDDLE_H
#define SEQUENCERIDDLE_H

#include "Riddle.h"

class SequenceRiddle : public Riddle {
public:
    SequenceRiddle(QString prompt, QString answer, QString hint, QString godName, Difficulty difficulty);

    QString getPrompt() const override;
    QString getAnswer() const override;
    QString getHint() const override;
    RiddleType getType() const override;
};

#endif
