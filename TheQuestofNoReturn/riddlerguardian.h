#ifndef RIDDLERGUARDIAN_H
#define RIDDLERGUARDIAN_H

#include "Guardiansprite.h"
class Room;

class Riddlerguardian : public GuardianSprite
{
public:
    explicit Riddlerguardian(Room* room = nullptr, QGraphicsItem* parent = nullptr);
    void startEncounter(Player* player) override;

private:
    Room* m_room = nullptr;
};

#endif // RIDDLERGUARDIAN_H
