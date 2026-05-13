#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include <QLabel>
#include <QSet>
#include <QVector>
#include <QList>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include "SharedEnums.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum class BossState { INACTIVE, SHOOTING, TIRED, DYING };

// ── Interactable object structures ────────────────────────
struct ClueFragment {
    QGraphicsPixmapItem *sprite;
    QGraphicsRectItem   *glowEffect;
    bool collected;
    float glowPhase;
};

struct Statue {
    QGraphicsPixmapItem *sprite;
    QGraphicsRectItem   *hitbox;
    QString loreText;
};

struct Tablet {
    QGraphicsPixmapItem *sprite;
    QGraphicsRectItem   *hitbox;
    QString hintText;
};

struct Torch {
    QGraphicsPixmapItem *sprite;
    QGraphicsRectItem   *hitbox;
    bool activated;
    QList<QGraphicsItem*> revealsItems;
};

struct HiddenRelic {
    QGraphicsPixmapItem *sprite;
    bool collected;
    bool visible;
};

struct Altar {
    QGraphicsPixmapItem *sprite;
    QGraphicsRectItem   *hitbox;
    QGraphicsRectItem   *glowEffect;
    QGraphicsTextItem   *hint;
    bool active;
};

struct DestructibleCover {
    QGraphicsRectItem *item;
    int hp;  // breaks when hp reaches 0
};

// ── Room-specific mechanic types ──────────────────────────
enum class RoomMechanic {
    NONE,
    TUTORIAL,
    STEALTH_GUARDIAN,
    FAKE_DOORS,
    SPIKE_TRAPS,
    HIDDEN_RELIC_PUZZLE,
    BOSS_FIGHT,
    COLLAPSING_FLOOR
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void onCountdownTick();
    void onGameLoop();

private:
    Ui::MainWindow *ui;

    // ── Player selection ──────────────────────────────────
    QString    selectedCharacter;
    QString    selectedTrait;
    QString    playerName;
    Difficulty selectedDifficulty;

    // ── Game state ────────────────────────────────────────
    QTimer *countdownTimer;
    QTimer *loopTimer;
    int     timeSeconds;
    int     lives;
    int     maxLives;
    int     score;
    int     currentRoom;
    bool    gameActive;
    bool    invincible;
    int     invFrames;
    bool    riddleOpen;
    int     animTick;
    int     animFrame;

    // ── Graphics scene ────────────────────────────────────
    struct Trap {
        QGraphicsRectItem   *item;
        QGraphicsPixmapItem *sprite;
        bool  horiz;
        float mn, mx, spd, dir;
        int   type;
        // Chase behavior
        float detectionRadius;
        float chaseSpeed;
        bool  chasing;
        float homeX, homeY;  // original patrol center
    };

    struct Crescent {
        QGraphicsPixmapItem *item;
        float vx, vy;
    };

    QGraphicsScene       *scene;
    QGraphicsPixmapItem  *playerSprite;
    QGraphicsRectItem    *doorItem;
    QGraphicsTextItem    *pressEHint;
    QList<QGraphicsItem*> roomItems;
    QVector<Trap>              traps;
    QVector<QGraphicsRectItem*> obstacles;

    // ── Interactables ─────────────────────────────────────
    QVector<ClueFragment>  clueFragments;
    QVector<Statue>        statues;
    QVector<Tablet>        tablets;
    QVector<Torch>         torches;
    QVector<HiddenRelic>   relics;
    Altar                  altar;
    int                    cluesFound;
    int                    requiredClues;
    int                    relicsCollected;
    int                    totalRelicsInGame;
    RoomMechanic           currentMechanic;

    // ── Room-specific mechanic data ───────────────────────
    QVector<QGraphicsRectItem*> fakeDoors;
    QGraphicsRectItem          *realDoor;
    QVector<QGraphicsRectItem*> spikeTraps;
    QVector<bool>               spikeActive;
    int                         spikeTimer;
    QVector<QGraphicsRectItem*> collapsingTiles;
    QVector<int>                collapseTriggerTimer;
    QVector<bool>               tileCollapsed;

    // ── Boss fight ────────────────────────────────────────
    QGraphicsPixmapItem *bossSprite;
    QGraphicsRectItem   *bossHPBar;       // red bar shrinks as boss takes damage
    BossState            bossState;
    int                  bossHP;          // 25 to start
    int                  bossShootCount;  // crescents fired this cycle (0–8)
    int                  bossShootTimer;  // frames between shots
    int                  bossTiredTimer;  // countdown frames in TIRED phase (600 = 10 s)
    int                  bossHitsThisPhase; // hits landed during current opening (max 5)
    int                  bossDyingTimer;  // death animation countdown
    QVector<Crescent>    crescents;
    QVector<DestructibleCover> destructibleCovers;

    // ── Input ─────────────────────────────────────────────
    QSet<int>  heldKeys;
    QLabel    *mapLabels[6];   // sidebar room labels (rooms 0–5; room 6 has no slot)

    // ── Game flow ─────────────────────────────────────────
    void startGame();
    void resetGame();
    void setupScene();
    void loadRoom(int room);
    void clearRoom();
    void tryInteract();
    void showRiddle();
    void onRiddleCorrect();
    void onRiddleWrong();
    void onTrapHit();
    void triggerGameOver(const QString &reason);
    void triggerWin();

    // ── Boss helpers ──────────────────────────────────────
    void updateBoss();
    void fireCrescent();
    void updateCrescents();
    void tryBossHit();

    // ── Scene helpers ─────────────────────────────────────
    void addTrap(qreal x, qreal y, qreal w, qreal h,
                 bool horiz, float mn, float mx, float spd,
                 float startDir = 1.f, int enemyType = 0,
                 float detectRadius = 0.f, float chaseSpd = 0.f);
    void addObstacle(qreal x, qreal y, qreal w, qreal h, int style = 0);
    QGraphicsRectItem* addRoomRect(qreal x, qreal y, qreal w, qreal h,
                                    QColor fill,
                                    QColor border = Qt::transparent,
                                    qreal bw = 0, int z = 1);
    QGraphicsTextItem* addRoomText(const QString &txt, qreal x, qreal y,
                                    QColor col, int sz, int z = 2);

    // ── Interactable helpers ──────────────────────────────
    void addClueFragment(qreal x, qreal y);
    void addStatue(qreal x, qreal y, const QString &lore);
    void addTablet(qreal x, qreal y, const QString &hint);
    void addTorch(qreal x, qreal y, QList<QGraphicsItem*> reveals = {});
    void addRelic(qreal x, qreal y, bool startVisible = false);
    void addAltarToRoom(qreal x, qreal y);
    void tryCollectClue();
    void tryInteractStatue();
    void tryInteractTablet();
    void tryInteractTorch();
    void tryCollectRelic();
    void checkAltarActivation();
    void updateClueGlow();
    void updateEnemyChase();

    // ── Room mechanic helpers ─────────────────────────────
    void addFakeDoor(qreal x, qreal y);
    void addSpikeTrap(qreal x, qreal y, qreal w, qreal h);
    void addCollapsingTile(qreal x, qreal y, qreal w, qreal h);
    void updateSpikeTraps();
    void updateCollapsingFloor();
    void checkFakeDoorCollision();
    void showRoomIntro(int room);

    // ── HUD helpers ───────────────────────────────────────
    void refreshTimer();
    void refreshLives();
    void refreshMap();
    void setEventText(const QString &msg);
};

#endif // MAINWINDOW_H
