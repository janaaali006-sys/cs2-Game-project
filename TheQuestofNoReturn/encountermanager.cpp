#include "encountermanager.h"
#include <QRandomGenerator>

bool Encountermanager::shouldSpawnGuardian(int level, Difficulty difficulty)
{
    if (level == 3 || level == 5)
        return true; // boss milestone levels

    const int roll = QRandomGenerator::global()->bounded(100);
    const int chance = (difficulty == Difficulty::EASY) ? 30 :
                       (difficulty == Difficulty::MEDIUM) ? 40 : 50;
    return roll < chance;
}

GuardianType Encountermanager::randomGuardianType()
{
    return static_cast<GuardianType>(QRandomGenerator::global()->bounded(3));
}
