#ifndef IDENTITYRIDDLE_H
#define IDENTITYRIDDLE_H

#include "Riddle.h"

class IdentityRiddle : public Riddle {
public:
    IdentityRiddle(QString prompt, QString answer, QString hint, QString godName, Difficulty difficulty);

    QString getPrompt() const override;
    QString getAnswer() const override;
    QString getHint() const override;
    RiddleType getType() const override;
};

#endif
