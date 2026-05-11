#ifndef ENCOUNTERMANAGER_H
#define ENCOUNTERMANAGER_H

#include "SharedEnums.h"

class Encountermanager
{
public:
    static bool shouldSpawnGuardian(int level, Difficulty difficulty);
    static GuardianType randomGuardianType();
};

#endif // ENCOUNTERMANAGER_H
