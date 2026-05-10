#ifndef RIDDLEALTAR_H
#define RIDDLEALTAR_H

#include <QObject>
#include "../GameObject.h"
#include "../riddles/Riddle.h"

class Player;
class Room;

// RiddleAltar — a GameObject that sits in the scene.
// When the player collides with it, it shows the RiddleDialog.
// On correct answer → Room::setRiddleSolved(true) → door unlocks.
class RiddleAltar : public GameObject {
    Q_OBJECT

public:
    RiddleAltar(Riddle* riddle, Room* room, QGraphicsItem* parent = nullptr)
        : GameObject(parent), m_riddle(riddle), m_room(room) {}

    void setRiddle(Riddle* riddle) { m_riddle = riddle; }

    void trigger(Player* player) override;
    bool isPassable() const override { return false; }
    QString objectName() const override { return "RiddleAltar"; }

private:
    Riddle* m_riddle = nullptr;
    Room*   m_room   = nullptr;
    bool    m_used   = false;
};

#endif
