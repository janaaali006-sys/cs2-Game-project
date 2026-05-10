#include "GoodOutcome.h"
#include "../player.h"

GoodOutcome::GoodOutcome(QString description, int scoreBonus)
    : m_description(description),
      m_scoreBonus(scoreBonus)
{
}

void GoodOutcome::apply(Player* player) {
    player->addScore(m_scoreBonus);
}

QString GoodOutcome::getDescription() const {
    return m_description;
}

bool GoodOutcome::isPositive() const {
    return true;
}

