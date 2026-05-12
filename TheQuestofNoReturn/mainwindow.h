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

    // ── Player selection (were anonymous globals before) ──
    QString    selectedCharacter;
    QString    selectedTrait;
    QString    playerName;
    Difficulty selectedDifficulty;  // was never stored before — diagnosis item 4

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
    };

    QGraphicsScene       *scene;
    QGraphicsPixmapItem  *playerSprite;
    QGraphicsRectItem    *doorItem;
    QGraphicsTextItem    *pressEHint;
    QList<QGraphicsItem*> roomItems;   // decorations + door + hint (not traps)
    QVector<Trap>              traps;
    QVector<QGraphicsRectItem*> obstacles;

    // ── Input ─────────────────────────────────────────────
    QSet<int>  heldKeys;
    QLabel    *mapLabels[6];           // sidebar room labels in order

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

    // ── Scene helpers ─────────────────────────────────────
    void addTrap(qreal x, qreal y, qreal w, qreal h,
                 bool horiz, float mn, float mx, float spd,
                 float startDir = 1.f, int enemyType = 0);
    void addObstacle(qreal x, qreal y, qreal w, qreal h, int style = 0);
    QGraphicsRectItem* addRoomRect(qreal x, qreal y, qreal w, qreal h,
                                    QColor fill,
                                    QColor border = Qt::transparent,
                                    qreal bw = 0, int z = 1);
    QGraphicsTextItem* addRoomText(const QString &txt, qreal x, qreal y,
                                    QColor col, int sz, int z = 2);

    // ── HUD helpers ───────────────────────────────────────
    void refreshTimer();
    void refreshLives();
    void refreshMap();
    void setEventText(const QString &msg);
};

#endif // MAINWINDOW_H
