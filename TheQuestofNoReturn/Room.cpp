#include "Room.h"
#include "PlayerSprite.h"
#include "DoorItem.h"
#include "TrapItem.h"
#include "artefactitem.h"
#include "watcherguardian.h"
#include "pursuerguardian.h"
#include "riddlerguardian.h"
#include "encountermanager.h"
#include "player.h"
#include "world/RiddleAltar.h"
#include "riddlegenerator.h"
#include "config.h"
#include <QFileInfo>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QRandomGenerator>
#include <QTimer>
#include <QVector>

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
    setStickyFocus(true);
    addBackground();
    addPlayer();
    addDoor();
    addTraps();
    addGuardian();
    addArtefacts();
    addAltar();

    // Advance timer — drives PlayerSprite::advance() (collision detection) at ~60fps
    QTimer* advanceTimer = new QTimer(this);
    connect(advanceTimer, &QTimer::timeout, this, &QGraphicsScene::advance);
    advanceTimer->start(16);
}

int      Room::levelNumber()    const { return m_levelNumber; }
QString  Room::godName()        const { return m_godName; }
QString  Room::chamberName()    const { return m_chamberName; }
RoomMood Room::mood()           const { return m_mood; }
bool     Room::hasBoss()        const { return m_hasBoss; }
bool     Room::isRiddleSolved() const { return m_riddleSolved; }

void Room::setRiddleSolved(bool solved) { m_riddleSolved = solved; }

void Room::completeRoom() { emit roomComplete(); }

void Room::focusPlayer()
{
    if (!m_playerSprite)
        return;

    setFocusItem(m_playerSprite);
    m_playerSprite->setFocus();
}

void Room::addBackground()
{
    QPixmap bg(m_backgroundPath);
    if (bg.isNull() && m_backgroundPath.startsWith(":/images/")) {
        const QString baseName = m_backgroundPath.mid(QString(":/images/").size());
        const QString sourceDir = QFileInfo(QString::fromUtf8(__FILE__)).absolutePath();
        bg.load(sourceDir + "/" + baseName);
    }
    if (bg.isNull()) {
        bg = QPixmap(900, 600);
        if (m_mood == RoomMood::CALM)        bg.fill(QColor("#1c3d5a"));
        else if (m_mood == RoomMood::CURSED) bg.fill(QColor("#5a1c1c"));
        else                                  bg.fill(QColor("#6b541b"));
    }
    QGraphicsPixmapItem* background = addPixmap(bg.scaled(900, 600));
    background->setZValue(-10);
}

void Room::addPlayer()
{
    m_playerSprite = new PlayerSprite(m_player);
    addItem(m_playerSprite);
    m_playerSprite->setPos(60, 280);
    m_playerSprite->setZValue(5);
    focusPlayer();
}

void Room::addDoor()
{
    m_door = new DoorItem(this);
    addItem(m_door);
    m_door->setPos(820, 280);
    m_door->setZValue(3);
}

void Room::addTraps()
{
    const int trapCount = (m_difficulty == Difficulty::HARD)   ? 3 :
                          (m_difficulty == Difficulty::MEDIUM) ? 2 : 1;

    const QVector<QPointF> trapPoints = {
        QPointF(300, 420), QPointF(530, 340), QPointF(700, 240)
    };

    for (int i = 0; i < trapCount && i < trapPoints.size(); ++i) {
        TrapItem* trap = new TrapItem(this);
        addItem(trap);
        trap->setPos(trapPoints[i]);
        trap->setZValue(2);
    }
}

void Room::addArtefacts()
{
    const int artefactCount = (m_mood == RoomMood::CURSED) ? 1 : 2;

    const QVector<QPointF> artefactPoints = {
        QPointF(250, 250), QPointF(620, 180), QPointF(760, 430)
    };

    for (int i = 0; i < artefactCount && i < artefactPoints.size(); ++i) {
        ArtefactType type = static_cast<ArtefactType>((m_levelNumber + i) % 5);
        ArtefactItem* item = new ArtefactItem(this, type);
        addItem(item);
        // Fixed visible lanes avoid overlap with altar/door and ensure artefacts are discoverable.
        item->setPos(artefactPoints[i]);
        item->setZValue(4);
    }
}

void Room::addGuardian()
{
    // Keep guardians rare and meaningful so rooms stay readable.
    if (!(m_levelNumber == 3 || m_levelNumber == 5 || m_levelNumber == 6))
        return;
    if (!Encountermanager::shouldSpawnGuardian(m_levelNumber, m_difficulty))
        return;

    GuardianType type = Encountermanager::randomGuardianType();
    GuardianSprite* guardian = nullptr;
    switch (type) {
    case GuardianType::WATCHER:
        guardian = new Watcherguardian(this);
        break;
    case GuardianType::PURSUER:
        guardian = new Pursuerguardian(this);
        break;
    case GuardianType::RIDDLER:
    default:
        guardian = new Riddlerguardian(this);
        break;
    }
    addItem(guardian);
    guardian->setPos(700, 300);
    guardian->setZValue(2);
    m_guardianSpawned = true;
}

void Room::addAltar()
{
    RiddleGenerator* gen = new RiddleGenerator(this);
    gen->setApiKey(GOOGLEAI_API_KEY);

    RiddleType type = static_cast<RiddleType>(
        QRandomGenerator::global()->bounded(3));

    Riddle* riddle = gen->generate(m_godName, m_difficulty, type, m_mood);
    if (!riddle) return;

    RiddleAltar* altar = new RiddleAltar(riddle, this);

    QPixmap altarPixmap(":/images/altar.png");
    if (altarPixmap.isNull()) {
        const QString sourceDir = QFileInfo(QString::fromUtf8(__FILE__)).absolutePath();
        altarPixmap.load(sourceDir + "/altar.png");
    }
    if (altarPixmap.isNull()) {
        altarPixmap = QPixmap(50, 60);
        altarPixmap.fill(QColor("#c8a951"));
    }
    altar->setPixmap(altarPixmap.scaled(50, 60));
    altar->setPos(430, 250);
    altar->setZValue(4);
    addItem(altar);
}
