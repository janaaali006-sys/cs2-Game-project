#ifndef PURSUERGUARDIAN_H
#define PURSUERGUARDIAN_H

#include "Guardiansprite.h"
class Room;

class Pursuerguardian : public GuardianSprite
{
public:
    explicit Pursuerguardian(Room* room = nullptr, QGraphicsItem* parent = nullptr);
    void startEncounter(Player* player) override;

private:
    Room* m_room = nullptr;
};

#endif // PURSUERGUARDIAN_H
