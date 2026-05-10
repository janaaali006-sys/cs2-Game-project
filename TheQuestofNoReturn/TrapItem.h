#pragma once
#include "GameObject.h"
class Room;
class TrapItem : public GameObject
{
    Q_OBJECT
public:
    explicit TrapItem(Room* room, QGraphicsItem* parent = nullptr);
    void trigger(Player* player) override;
    QString objectName() const override;
private:
    void flashEffect();
private:
    Room* m_room;
};
