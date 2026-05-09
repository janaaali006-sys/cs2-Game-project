#include "IdentityRiddle.h"

IdentityRiddle::IdentityRiddle(QString prompt, QString answer, QString hint, QString godName, Difficulty difficulty)
    : Riddle(prompt, answer, hint, RiddleType::IDENTITY, godName, difficulty)
{
}

QString IdentityRiddle::getPrompt() const {
    return m_prompt;
}

QString IdentityRiddle::getAnswer() const {
    return m_answer;
}

QString IdentityRiddle::getHint() const {
    return m_hint;
}

RiddleType IdentityRiddle::getType() const {
    return RiddleType::IDENTITY;
}
