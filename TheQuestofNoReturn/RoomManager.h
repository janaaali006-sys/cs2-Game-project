#pragma once
#include <QObject>
#include <QVector>
#include <QGraphicsView>
#include "SharedEnums.h"
class Room;
class Player;
class RoomManager : public QObject
{
    Q_OBJECT
public:
    explicit RoomManager(QGraphicsView* view, Player* player, QObject* parent = nullptr);
    ~RoomManager();
    void startGame();
    void loadNextRoom();
    bool isFinalLevel() const;
    Room* currentRoom() const;
    int currentLevel() const;
signals:
    void gameWon();
private:
    Room* buildRoom(int level);
    RoomMood randomMood(int level) const;
private:
    QVector<Room*> m_rooms;
    QGraphicsView* m_view;
    Player* m_player;
    int m_currentLevel = 0;
};
