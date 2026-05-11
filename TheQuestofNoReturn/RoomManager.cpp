#include "RoomManager.h"
#include "Room.h"
#include "player.h"
#include <QRandomGenerator>
#include <QPainter>
#include <QTimer>

namespace {
struct RoomBlueprint {
    QString godName;
    QString chamberName;
    bool hasBoss = false;
};

QVector<RoomBlueprint> setupRoomBlueprints()
{
    return {
        {"Horus", "Courtyard of Horus", false},
        {"Anubis", "Chamber of Anubis", false},
        {"Ra", "Hall of Ra", true},
        {"Sphinx", "Pyramid Vault", false},
        {"Osiris", "Chamber of Osiris", true},
        {"All", "The Ankh Chamber", true}
    };
}
}

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
        m_view->setRenderHint(QPainter::Antialiasing);
        m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_view->setFocusPolicy(Qt::StrongFocus);
        m_view->fitInView(room->sceneRect(), Qt::KeepAspectRatio);
        m_view->show();
        m_view->setFocus();
        room->focusPlayer();
        QTimer::singleShot(0, this, [this, room]() {
            if (m_view) m_view->setFocus();
            room->focusPlayer();
        });
        // Re-fit once layout settles so scene fills the final view size.
        QTimer::singleShot(30, this, [this, room]() {
            if (!m_view || m_view->scene() != room) return;
            m_view->fitInView(room->sceneRect(), Qt::KeepAspectRatio);
            m_view->setFocus();
            room->focusPlayer();
        });
    }

    if (m_player) {
        m_player->resetHintForLevel();
    }

    emit roomEntered(m_currentLevel, room->chamberName(), room->mood(), room->guardianSpawned());
}

bool RoomManager::isFinalLevel() const
{
    return m_currentLevel >= 6;
}

Room* RoomManager::currentRoom() const
{
    if (m_rooms.isEmpty()) return nullptr;
    return m_rooms.last();
}

int RoomManager::currentLevel() const
{
    return m_currentLevel;
}

Room* RoomManager::buildRoom(int level)
{
    const QVector<RoomBlueprint> blueprints = setupRoomBlueprints();
    const int idx = level - 1;
    const bool validIndex = idx >= 0 && idx < blueprints.size();
    const RoomBlueprint roomSetup = validIndex
        ? blueprints[idx]
        : RoomBlueprint{"Unknown", "Unknown Chamber", false};

    RoomMood mood = randomMood(level);
    return new Room(level,
                    roomSetup.godName,
                    roomSetup.chamberName,
                    mood,
                    roomSetup.hasBoss,
                    m_player,
                    m_player->getDifficulty());
}

RoomMood RoomManager::randomMood(int level) const
{
    if (level == 6) return RoomMood::BLESSED;
    int value = QRandomGenerator::global()->bounded(3);
    if (value == 0) return RoomMood::CALM;
    if (value == 1) return RoomMood::CURSED;
    return RoomMood::BLESSED;
}
