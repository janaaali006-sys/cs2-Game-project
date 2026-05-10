#ifndef RIDDLEALTAR_H
#define RIDDLEALTAR_H

#include <QObject>
#include "../GameObject.h"
#include "../riddles/Riddle.h"

class Player;
class Room;

class RiddleAltar : public QObject, public GameObject {
    Q_OBJECT

public:
    RiddleAltar(Riddle* riddle, Room* room);

    void trigger(Player* player) override;
    bool isPassable() const override { return false; }
    QString objectName() const override { return "RiddleAltar"; }
    void setRiddle(Riddle* riddle) { m_riddle = riddle; }

signals:
    void riddleTriggered(Riddle* riddle);

private:
    Riddle* m_riddle;
    Room* m_room;
    bool m_used = false;
};

#endif
