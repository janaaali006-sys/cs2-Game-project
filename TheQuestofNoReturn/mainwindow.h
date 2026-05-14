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

#include "gamedata.h"       // PlayerData, GameState, BossState, RoomMechanic, ROOMS
#include "gameobject.h"     // abstract GameObject base
#include "enemy.h"          // Enemy, Mummy, Specter, Scarab, DarkMummy
#include "interactable.h"   // Interactable, ClueFragmentObj, AltarObj, …
#include "gamehud.h"        // GameHUD (UI-only class)

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

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

    // ── Data: player selection and mutable game state ─────────────────────────
    PlayerData m_player;   // character, trait, name, difficulty
    GameState  m_state;    // lives, score, room, timers, boss sub-state, etc.

    // ── UI: HUD wrapper ───────────────────────────────────────────────────────
    GameHUD   *m_hud;      // owns refreshTimer/refreshLives/refreshMap/setEventText/setScore

    // ── Timers ────────────────────────────────────────────────────────────────
    QTimer *countdownTimer;
    QTimer *loopTimer;

    // ── Graphics scene ────────────────────────────────────────────────────────
    // Boss projectile (simple data — no dedicated class needed)
    struct Crescent {
        QGraphicsPixmapItem *item;
        float vx, vy;
    };

    // Destructible cover in boss room
    struct DestructibleCover {
        QGraphicsRectItem *item;
        int hp;
    };

    QGraphicsScene       *scene;
    QGraphicsPixmapItem  *playerSprite;
    QGraphicsRectItem    *doorItem;
    QGraphicsTextItem    *pressEHint;
    QList<QGraphicsItem*> roomItems;

    // ── Game objects — polymorphic containers ─────────────────────────────────
    QVector<Enemy*>        m_enemies;       // Mummy, Specter, Scarab, DarkMummy
    QVector<Interactable*> m_interactables; // ClueFragmentObj…AltarObj (priority order)
    AltarObj              *m_altar;         // convenience pointer; also inside m_interactables

    // ── Solid obstacles (no game-object behaviour needed) ────────────────────
    QVector<QGraphicsRectItem*> obstacles;

    // ── Room-specific mechanic items (raw Qt items; no polymorphism needed) ──
    QVector<QGraphicsRectItem*> fakeDoors;
    QGraphicsRectItem          *realDoor;
    QVector<QGraphicsRectItem*> spikeTraps;
    QVector<bool>               spikeActive;
    QVector<QGraphicsRectItem*> collapsingTiles;
    QVector<int>                collapseTriggerTimer;
    QVector<bool>               tileCollapsed;

    // ── Boss fight items ──────────────────────────────────────────────────────
    QGraphicsPixmapItem        *bossSprite;
    QGraphicsRectItem          *bossHPBar;
    QVector<Crescent>           crescents;
    QVector<DestructibleCover>  destructibleCovers;

    // ── Input ─────────────────────────────────────────────────────────────────
    QSet<int>  heldKeys;
    QLabel    *m_mapLabels[6];

    // ── Game flow ─────────────────────────────────────────────────────────────
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

    // ── Boss helpers ──────────────────────────────────────────────────────────
    void updateBoss();
    void fireCrescent();
    void updateCrescents();
    void tryBossHit();

    // ── Scene helpers ─────────────────────────────────────────────────────────
    void addEnemy(qreal x, qreal y, qreal w, qreal h,
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

    // ── Interactable helpers ──────────────────────────────────────────────────
    void addClueFragment(qreal x, qreal y);
    void addStatue(qreal x, qreal y, const QString &lore);
    void addTablet(qreal x, qreal y, const QString &hint);
    void addTorch(qreal x, qreal y, QList<QGraphicsItem*> reveals = {});
    void addRelic(qreal x, qreal y, bool startVisible = false);
    void addAltarToRoom(qreal x, qreal y);
    void checkAltarActivation();

    // ── Room mechanic helpers ─────────────────────────────────────────────────
    void addFakeDoor(qreal x, qreal y);
    void addSpikeTrap(qreal x, qreal y, qreal w, qreal h);
    void addCollapsingTile(qreal x, qreal y, qreal w, qreal h);
    void updateSpikeTraps();
    void updateCollapsingFloor();
    void checkFakeDoorCollision();
    void showRoomIntro(int room);
};

#endif // MAINWINDOW_H
