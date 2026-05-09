#include "HintOutcome.h"
#include "../entities/Player.h"

HintOutcome::HintOutcome(QString description)
    : m_description(description)
{
}

void HintOutcome::apply(Player* player) {
    player->setHintAvailable(true);
}

QString HintOutcome::getDescription() const {
    return m_description;
}

bool HintOutcome::isPositive() const {
    return true;
}
