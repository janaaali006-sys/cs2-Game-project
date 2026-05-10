#include "Room.h"
#include "PlayerSprite.h"
#include "DoorItem.h"
#include "TrapItem.h"
#include "player.h"
#include "world/RiddleAltar.h"
#include "riddlegenerator.h"
#include "config.h"
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QRandomGenerator>

Room::Room(int levelNumber,
           const QString& godName,
           const QString& chamberName,
           RoomMood mood,
           bool hasBoss,
           Player* player,
           Difficulty difficulty,
           QObject* parent)
    : QGraphicsScene(parent),
      m_levelNumber(levelNumber),
      m_godName(godName),
      m_chamberName(chamberName),
      m_mood(mood),
      m_difficulty(difficulty),
      m_hasBoss(hasBoss),
      m_player(player)
{
    setSceneRect(0, 0, 900, 600);
    m_backgroundPath = QString(":/images/room%1.png").arg(levelNumber);
}

void Room::setup()
{
    clear();
    addBackground();
    addPlayer();
    addDoor();
    addTraps();
    addAltar();
}

int     Room::levelNumber()    const { return m_levelNumber; }
QString Room::godName()        const { return m_godName; }
QString Room::chamberName()    const { return m_chamberName; }
RoomMood Room::mood()          const { return m_mood; }
bool    Room::hasBoss()        const { return m_hasBoss; }
bool    Room::isRiddleSolved() const { return m_riddleSolved; }

void Room::setRiddleSolved(bool solved) { m_riddleSolved = solved; }

void Room::completeRoom() { emit roomComplete(); }

void Room::addBackground()
{
    QPixmap bg(m_backgroundPath);
    if (bg.isNull()) {
        bg = QPixmap(900, 600);
        if (m_mood == RoomMood::CALM)         bg.fill(QColor("#1c3d5a"));
        else if (m_mood == RoomMood::CURSED)  bg.fill(QColor("#5a1c1c"));
        else                                   bg.fill(QColor("#6b541b"));
    }
    QGraphicsPixmapItem* background = addPixmap(bg.scaled(900, 600));
    background->setZValue(-10);
}

void Room::addPlayer()
{
    m_playerSprite = new PlayerSprite(m_player);
    addItem(m_playerSprite);
    m_playerSprite->setPos(60, 280);
    m_playerSprite->setFocus();
}

void Room::addDoor()
{
    m_door = new DoorItem(this);
    addItem(m_door);
    m_door->setPos(820, 280);
}

void Room::addTraps()
{
    int trapCount = (m_difficulty == Difficulty::HARD) ? 5 :
                    (m_difficulty == Difficulty::MEDIUM) ? 3 : 1;
    for (int i = 0; i < trapCount; ++i) {
        TrapItem* trap = new TrapItem(this);
        addItem(trap);
        int x = QRandomGenerator::global()->bounded(150, 750);
        int y = QRandomGenerator::global()->bounded(100, 500);
        trap->setPos(x, y);
    }
}

void Room::addAltar()
{
    RiddleGenerator* gen = new RiddleGenerator(this);
    gen->setApiKey(CLAUDE_API_KEY);

    RiddleType type = static_cast<RiddleType>(
        QRandomGenerator::global()->bounded(3));

    Riddle* riddle = gen->generate(m_godName, m_difficulty, type, m_mood);
    if (!riddle) return;

    RiddleAltar* altar = new RiddleAltar(riddle, this);

    QPixmap altarPixmap(":/images/altar.png");
    if (altarPixmap.isNull()) {
        altarPixmap = QPixmap(50, 60);
        altarPixmap.fill(QColor("#c8a951"));
    }
    altar->setPixmap(altarPixmap.scaled(50, 60));
    altar->setPos(400, 260);
    altar->setZValue(1);
    addItem(altar);
}
