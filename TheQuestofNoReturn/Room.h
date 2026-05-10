#pragma once
#include <QGraphicsScene>
#include <QString>
#include "SharedEnums.h"

class Player;
class PlayerSprite;
class DoorItem;

class Room : public QGraphicsScene
{
    Q_OBJECT
public:
    Room(int levelNumber,
         const QString& godName,
         const QString& chamberName,
         RoomMood mood,
         bool hasBoss,
         Player* player,
         Difficulty difficulty,        // needed by RiddleGenerator
         QObject* parent = nullptr);

    void setup();

    int     levelNumber()   const;
    QString godName()       const;
    QString chamberName()   const;
    RoomMood mood()         const;
    bool    hasBoss()       const;
    bool    isRiddleSolved()const;
    void    setRiddleSolved(bool solved);
    void    completeRoom();

signals:
    void roomComplete();

private:
    void addBackground();
    void addPlayer();
    void addDoor();
    void addTraps();
    void addAltar();        // Person 5 addition — places RiddleAltar in scene

private:
    int        m_levelNumber;
    QString    m_godName;
    QString    m_chamberName;
    RoomMood   m_mood;
    Difficulty m_difficulty;           // stored for RiddleGenerator
    bool       m_hasBoss;
    bool       m_riddleSolved = false;
    QString    m_backgroundPath;
    Player*    m_player;
    PlayerSprite* m_playerSprite = nullptr;
    DoorItem*     m_door         = nullptr;
};
