#pragma once
#include "GameObject.h"
class Room;
class DoorItem : public GameObject
{
    Q_OBJECT
public:
    explicit DoorItem(Room* room, QGraphicsItem* parent = nullptr);
    void trigger(Player* player) override;
    bool isPassable() const override;
    QString objectName() const override;
private:
    Room* m_room;
};
