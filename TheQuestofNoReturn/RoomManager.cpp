#include "RoomManager.h"
#include "Room.h"
#include "player.h"
#include <QRandomGenerator>
RoomManager::RoomManager(QGraphicsView* view, Player* player, QObject* parent)
    : QObject(parent), m_view(view), m_player(player)
{
}
RoomManager::~RoomManager()
{
    qDeleteAll(m_rooms);
    m_rooms.clear();
}
void RoomManager::startGame()
{
    m_currentLevel = 0;
    loadNextRoom();
}
void RoomManager::loadNextRoom()
{
    if (isFinalLevel()) {
        emit gameWon();
        return;
    }
    m_currentLevel++;
    Room* room = buildRoom(m_currentLevel);
    m_rooms.append(room);
    connect(room, &Room::roomComplete, this, &RoomManager::loadNextRoom);
    room->setup();
    if (m_view) {
        m_view->setScene(room);
        m_view->setFocus();
    }
    if (m_player) {
        m_player->resetHintForLevel();
    }
}
bool RoomManager::isFinalLevel() const
{
    return m_currentLevel >= 6;
}
Room* RoomManager::currentRoom() const
{
    if (m_rooms.isEmpty())
        return nullptr;
    return m_rooms.last();
}
int RoomManager::currentLevel() const
{
    return m_currentLevel;
}
Room* RoomManager::buildRoom(int level)
{
    QString godName;
    QString chamberName;
    bool hasBoss = false;
    switch (level) {
    case 1:
        godName = "Horus";
        chamberName = "Courtyard of Horus";
        hasBoss = false;
        break;
    case 2:
        godName = "Anubis";
        chamberName = "Chamber of Anubis";
        hasBoss = false;
        break;
    case 3:
        godName = "Ra";
        chamberName = "Hall of Ra";
        hasBoss = true;
        break;
    case 4:
        godName = "Sphinx";
        chamberName = "Pyramid Vault";
        hasBoss = false;
        break;
    case 5:
        godName = "Osiris";
        chamberName = "Chamber of Osiris";
        hasBoss = true;
        break;
    case 6:
        godName = "All";
        chamberName = "The Ankh Chamber";
        hasBoss = true;
        break;
    default:
        godName = "Unknown";
        chamberName = "Unknown Chamber";
        hasBoss = false;
        break;
    }
    RoomMood mood = randomMood(level);
    Room* room = new Room(level, godName, chamberName, mood, hasBoss, m_player, m_player->getDifficulty());
    return room;
}
RoomMood RoomManager::randomMood(int level) const
{
    if (level == 6) {
        return RoomMood::BLESSED;
    }
    int value = QRandomGenerator::global()->bounded(3);
    if (value == 0)
        return RoomMood::CALM;
    else if (value == 1)
        return RoomMood::CURSED;
    else
        return RoomMood::BLESSED;
}
