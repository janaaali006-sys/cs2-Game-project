#include "Riddle.h"

Riddle::Riddle(QString prompt, QString answer, QString hint, RiddleType type, QString godName, Difficulty difficulty)
    : m_prompt(prompt),
      m_answer(answer),
      m_hint(hint),
      m_godName(godName),
      m_difficulty(difficulty)
{
}

bool Riddle::checkAnswer(QString input) const {
    return input.trimmed().toLower() == m_answer.trimmed().toLower();
}

QString Riddle::getGodName() const {
    return m_godName;
}

Difficulty Riddle::getDifficulty() const {
    return m_difficulty;
}
