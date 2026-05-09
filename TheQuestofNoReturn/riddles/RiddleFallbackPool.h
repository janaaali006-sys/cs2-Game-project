#ifndef RIDDLEFALLBACKPOOL_H
#define RIDDLEFALLBACKPOOL_H

#include <QVector>
#include <QMap>
#include "Riddle.h"
#include "IdentityRiddle.h"
#include "LogicRiddle.h"
#include "SequenceRiddle.h"

class RiddleFallbackPool {
public:
    RiddleFallbackPool();

    void populate();
    Riddle* getRandom(RiddleType type, QString godName);

private:
    QMap<RiddleType, QVector<Riddle*>> m_pool;
};

#endif
