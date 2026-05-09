#include "LoseLifeOutcome.h"
#include "../entities/Player.h"

LoseLifeOutcome::LoseLifeOutcome(QString description)
    : m_description(description)
{
}

void LoseLifeOutcome::apply(Player* player) {
    player->loseLife();
}

QString LoseLifeOutcome::getDescription() const {
    return m_description;
}

bool LoseLifeOutcome::isPositive() const {
    return false;
}
