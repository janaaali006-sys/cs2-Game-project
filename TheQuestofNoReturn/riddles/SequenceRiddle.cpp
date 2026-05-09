#include "SequenceRiddle.h"

SequenceRiddle::SequenceRiddle(QString prompt, QString answer, QString hint, QString godName, Difficulty difficulty)
    : Riddle(prompt, answer, hint, RiddleType::SEQUENCE, godName, difficulty)
{
}

QString SequenceRiddle::getPrompt() const {
    return m_prompt;
}

QString SequenceRiddle::getAnswer() const {
    return m_answer;
}

QString SequenceRiddle::getHint() const {
    return m_hint;
}

RiddleType SequenceRiddle::getType() const {
    return RiddleType::SEQUENCE;
}
