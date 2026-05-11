#ifndef WATCHERGUARDIAN_H
#define WATCHERGUARDIAN_H

#include "Guardiansprite.h"
class Room;

class Watcherguardian : public GuardianSprite
{
public:
    explicit Watcherguardian(Room* room = nullptr, QGraphicsItem* parent = nullptr);
    void startEncounter(Player* player) override;

private:
    Room* m_room = nullptr;
};

#endif // WATCHERGUARDIAN_H
