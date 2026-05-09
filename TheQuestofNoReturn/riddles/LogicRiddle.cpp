#include "LogicRiddle.h"

LogicRiddle::LogicRiddle(QString prompt, QString answer, QString hint, QString godName, Difficulty difficulty)
    : Riddle(prompt, answer, hint, RiddleType::LOGIC, godName, difficulty)
{
}

QString LogicRiddle::getPrompt() const {
    return m_prompt;
}

QString LogicRiddle::getAnswer() const {
    return m_answer;
}

QString LogicRiddle::getHint() const {
    return m_hint;
}

RiddleType LogicRiddle::getType() const {
    return RiddleType::LOGIC;
}
