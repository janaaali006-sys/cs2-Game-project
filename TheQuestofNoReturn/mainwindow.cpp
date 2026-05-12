#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pausemenudialog.h"
#include "difficultydialog.h"
#include "howtoplaydialog.h"
#include "riddledialog.h"

#include <QKeyEvent>
#include <QPainter>
#include <QtMath>
#include <QRandomGenerator>

static QPixmap createPlayerSprite(int frame = 0)
{
    QPixmap pix(55, 75);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    const QColor hat  ( 52,  28,   7);
    const QColor band (190, 145,  40);
    const QColor skin (235, 185, 110);
    const QColor eye  ( 25,  12,   0);
    const QColor shirt( 75, 100, 145);
    const QColor pants( 48,  32,  12);
    const QColor boot ( 35,  18,   4);

    // Cowboy hat dome
    p.fillRect(14,  2, 27, 14, hat);
    // Hat brim (wide)
    p.fillRect( 2, 12, 51,  7, hat);
    // Hat band
    p.fillRect(14, 13, 27,  3, band);

    // Face
    p.fillRect(17, 20, 21, 17, skin);
    // Eyes
    p.fillRect(21, 25,  3,  3, eye);
    p.fillRect(31, 25,  3,  3, eye);
    // Mouth
    p.fillRect(23, 34,  9,  2, QColor(170, 70, 50));

    // Neck
    p.fillRect(23, 37,  9,  5, skin);

    // Torso
    p.fillRect(14, 42, 27, 16, shirt);
    // Left arm
    p.fillRect( 5, 42, 10, 14, shirt);
    // Right arm
    p.fillRect(40, 42, 10, 14, shirt);
    // Hands
    p.fillRect( 5, 55, 10,  5, skin);
    p.fillRect(40, 55, 10,  5, skin);

    // Belt
    p.fillRect(14, 58, 27,  3, QColor(50, 28, 6));
    // Buckle
    p.fillRect(24, 58,  7,  3, band);

    // Legs and boots: frame 0 = neutral, frame 1 = stride (legs spread)
    int lo = (frame == 0) ? 0 : 3;   // outward offset per leg
    p.fillRect(15 - lo, 61, 11, 12, pants);
    p.fillRect(29 + lo, 61, 11, 12, pants);
    p.fillRect(13 - lo, 70, 14,  5, boot);
    p.fillRect(28 + lo, 70, 14,  5, boot);

    p.end();
    return pix;
}

// ═══════════════════════════════════════════════════════
// ROOM NARRATIVE
// ═══════════════════════════════════════════════════════

struct RoomInfo {
    QString name;
    QString enterText;   // shown in eventLabel when room loads
    QString solvedText;  // shown when riddle is answered correctly
    bool    survivorHeals;
    int     baseScore;
    int     bonusScore;
};

static const RoomInfo ROOMS[6] = {
    { "Entrance Chamber",
      "Ancient glyphs surround you. Reach the golden door and press E.",
      "The seal breaks — the passage is open!",
      false, 100, 60 },
    { "Guardian Hall",
      "A stone guardian patrols the hall. Dodge it and reach the door.",
      "The colossus crumbles — you may pass!",
      false, 120, 60 },
    { "Hall of Echoes",
      "Ethereal phantoms drift across the hall. Weave through and reach the door.",
      "The echoes answer your wisdom — the path clears!",
      false, 130, 60 },
    { "Trial Room",
      "Ancient mechanisms sweep the floor. Navigate them and answer the tablet.",
      "The stone tablet shatters — trial complete!",
      false, 150, 70 },
    { "Ankh Sanctuary",
      "Dark energy circles the altar. Avoid it and claim the Ankh.",
      "The Ankh glows — it has chosen you!",
      true, 140, 60 },
    { "Anubis Gate",
      "Anubis stands at the final gate. Prove your worth — reach him and answer.",
      "Anubis bows. The gate opens — you have conquered the tomb!",
      false, 200, 80 }
};

// ═══════════════════════════════════════════════════════
// RIDDLES
// ═══════════════════════════════════════════════════════

static const RiddleDialog::Riddle RIDDLES[6] = {
    { "A seal on the passage demands:\n\n"
      "\"I speak without a mouth, hear without ears,\n"
      "have no body, yet come alive with wind. What am I?\"",
      {"A Shadow", "An Echo", "The Desert Wind", "A Torch Flame"}, 1,
      "Scholar Insight: Think of sound bouncing off these ancient stone walls..." },

    { "The stone guardian intones:\n\n"
      "\"The more you take, the more you leave behind. What am I?\"",
      {"Time", "Sand", "Footsteps", "Gold"}, 2,
      "Scholar Insight: Consider what remains on the ground with each step..." },

    { "Glowing runes whisper:\n\n"
      "\"I have cities, but no houses.\nI have mountains, but no trees.\n"
      "I have water, but no fish. What am I?\"",
      {"A Dream", "A Map", "A Painting", "A Stone Tablet"}, 1,
      "Scholar Insight: Explorers carry this to navigate the unknown..." },

    { "The stone tablet blazes:\n\n"
      "\"What has keys but no locks,\nspace but no room,\n"
      "and you can enter — but not go inside?\"",
      {"A House", "A Keyboard", "A Chest", "A Gateway"}, 1,
      "Scholar Insight: In the modern world, you type on this every day..." },

    { "The Ankh floats and asks:\n\n"
      "\"The Ankh is the symbol of eternal life.\n"
      "Which two basic shapes combine to form it?\"",
      {"A circle and a cross", "A triangle and a line",
       "A star and a crescent", "Two crossing lines"}, 0,
      "Scholar Insight: Trace the Ankh on this altar — find its geometric outline..." },

    { "Anubis speaks:\n\n"
      "\"I weigh the heart against the Feather of Ma'at.\n"
      "Which beast devours the hearts of the unworthy\n"
      "in the Hall of Two Truths?\"",
      {"The Sphinx", "Sobek", "Ammit", "Apep"}, 2,
      "Scholar Insight: It has the head of a crocodile, body of a lion,\n"
      "and hindquarters of a hippopotamus..." }
};

// ═══════════════════════════════════════════════════════
// ROOM-MAP SIDEBAR STYLES
// ═══════════════════════════════════════════════════════

static const QString S_NORMAL =
    "QLabel{background:rgba(15,10,5,180);color:#e2cfa0;"
    "border-left:3px solid #c8a03c;border-radius:6px;"
    "padding-left:8px;font-size:12px;font-family:\"Copperplate\";}";

static const QString S_ACTIVE =
    "QLabel{background:rgba(200,160,60,210);color:#0d0a05;"
    "border-left:4px solid #ffffc0;border-radius:6px;"
    "padding-left:8px;font-size:12px;font-family:\"Copperplate\";font-weight:bold;}";

static const QString S_DONE =
    "QLabel{background:rgba(0,80,20,160);color:#90ffb0;"
    "border-left:3px solid #00cc44;border-radius:6px;"
    "padding-left:8px;font-size:12px;font-family:\"Copperplate\";}";


// ═══════════════════════════════════════════════════════
// CONSTRUCTOR
// ═══════════════════════════════════════════════════════

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , selectedCharacter("Zara")
    , selectedTrait("Scholar")
    , playerName("")
    , selectedDifficulty(Difficulty::MEDIUM)
    , timeSeconds(0)
    , lives(0)
    , maxLives(0)
    , score(0)
    , currentRoom(0)
    , gameActive(false)
    , invincible(false)
    , invFrames(0)
    , riddleOpen(false)
    , animTick(0)
    , animFrame(0)
    , scene(nullptr)
    , playerSprite(nullptr)
    , doorItem(nullptr)
    , pressEHint(nullptr)
{
    ui->setupUi(this);

    // ── Timers ──────────────────────────────────────────
    countdownTimer = new QTimer(this);
    countdownTimer->setInterval(1000);
    connect(countdownTimer, &QTimer::timeout, this, &MainWindow::onCountdownTick);

    loopTimer = new QTimer(this);
    loopTimer->setInterval(16);   // ≈ 60 fps
    connect(loopTimer, &QTimer::timeout, this, &MainWindow::onGameLoop);

    // ── Room-map sidebar labels (index matches room order) ─
    mapLabels[0] = ui->label_7;   // Entrance Chamber
    mapLabels[1] = ui->label_10;  // Guardian Hall
    mapLabels[2] = ui->label_12;  // Hall of Echoes
    mapLabels[3] = ui->label_11;  // Trial Room
    mapLabels[4] = ui->label_8;   // Ankh Sanctuary
    mapLabels[5] = ui->label_9;   // Anubis Gate

    // ── Prevent QGraphicsView from stealing key focus ───
    ui->graphicsView->setFocusPolicy(Qt::NoFocus);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setInteractive(false);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);

    // ── Start page ──────────────────────────────────────
    ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);

    // ── Image scaling ───────────────────────────────────
    ui->portraitLabel->setScaledContents(true);
    ui->traitIconLabel->setScaledContents(true);
    ui->hudPortraitLabel->setScaledContents(true);
    ui->inventorySlot1->setScaledContents(true);
    ui->inventorySlot2->setScaledContents(true);
    ui->inventorySlot3->setScaledContents(true);
    ui->inventorySlot4->setScaledContents(true);

    // ── Character setup defaults ────────────────────────
    ui->portraitLabel->setPixmap(QPixmap(":/images/Zara.png"));
    ui->traitIconLabel->setPixmap(QPixmap(":/images/scholarIcon.png"));
    ui->descriptionLabel->setText("Fearless explorer driven by curiosity and courage.");
    ui->traitDescriptionLabel->setText("Gain intelligence bonuses and uncover hidden clues.");

    // ── HUD defaults (will be overwritten by startGame) ─
    ui->hudPortraitLabel->setPixmap(QPixmap(":/images/Zara.png"));
    ui->hudNameLabel->setText("ZARA");
    ui->hudTraitLabel->setText("SCHOLAR");
    ui->livesLabel->setText("♥ ♥ ♥");
    ui->timerLabel->setText("08:00");
    ui->scoreLabel->setText("🏆 0");
    ui->levelLabel->setText("1 / 6");
    ui->roomNameLabel->setText("Temple Entrance");
    ui->eventLabel->setText("A cold wind echoes through the chamber...");
    ui->controlsLabel->setText("WASD Move | E Interact | ESC Pause");

    // ── Inventory slots ─────────────────────────────────
    ui->inventorySlot1->setText(""); ui->inventorySlot1->setPixmap(QPixmap());
    ui->inventorySlot2->setText(""); ui->inventorySlot2->setPixmap(QPixmap());
    ui->inventorySlot3->setText(""); ui->inventorySlot3->setPixmap(QPixmap());
    ui->inventorySlot4->setText(""); ui->inventorySlot4->setPixmap(QPixmap());

    // ── Game-over / win backgrounds ─────────────────────
    ui->gameOverBackground->setPixmap(QPixmap(":/images/gameover_background.png"));
    ui->gameOverBackground->setScaledContents(true);
    ui->winBackgroundLabel->setPixmap(QPixmap(":/images/winBackground.png"));
    ui->winBackgroundLabel->setScaledContents(true);
    ui->ankhLabel->setPixmap(QPixmap(":/images/ankh.png"));
    ui->ankhLabel->setScaledContents(true);


    // ═══════════════════════════════════════════════════
    // MAIN MENU BUTTONS
    // ═══════════════════════════════════════════════════

    connect(ui->startButton, &QPushButton::clicked,
            this, [=]() {
                ui->stackedWidget->setCurrentWidget(ui->characterSetupScreen);
            });

    connect(ui->exitButton, &QPushButton::clicked,
            this, [=]() {
                close();
            });


    // ═══════════════════════════════════════════════════
    // CHARACTER SELECTION
    // ═══════════════════════════════════════════════════

    connect(ui->zaraButton, &QPushButton::clicked,
            this, [=]() {
                selectedCharacter = "Zara";
                ui->portraitLabel->setPixmap(QPixmap(":/images/Zara.png"));
                ui->descriptionLabel->setText("Fearless explorer driven by curiosity and courage.");
            });

    connect(ui->karimButton, &QPushButton::clicked,
            this, [=]() {
                selectedCharacter = "Karim";
                ui->portraitLabel->setPixmap(QPixmap(":/images/Karim.png"));
                ui->descriptionLabel->setText("Balanced adventurer skilled in combat and survival.");
            });

    connect(ui->nourButton, &QPushButton::clicked,
            this, [=]() {
                selectedCharacter = "Nour";
                ui->portraitLabel->setPixmap(QPixmap(":/images/NOOR.png"));
                ui->descriptionLabel->setText("Strategic thinker who uncovers hidden ancient secrets.");
            });

    connect(ui->ramiButton, &QPushButton::clicked,
            this, [=]() {
                selectedCharacter = "Rami";
                ui->portraitLabel->setPixmap(QPixmap(":/images/RAMI.png"));
                ui->descriptionLabel->setText("Fast and daring explorer who trusts instinct.");
            });


    // ═══════════════════════════════════════════════════
    // TRAIT SELECTION
    // ═══════════════════════════════════════════════════

    connect(ui->scholarButton, &QPushButton::clicked,
            this, [=]() {
                selectedTrait = "Scholar";
                ui->traitDescriptionLabel->setText("Gain intelligence bonuses and uncover hidden clues.");
                ui->traitIconLabel->setPixmap(QPixmap(":/images/scholarIcon.png"));
            });

    connect(ui->survivorButton, &QPushButton::clicked,
            this, [=]() {
                selectedTrait = "Survivor";
                ui->traitDescriptionLabel->setText("Higher endurance and resistance to danger.");
                ui->traitIconLabel->setPixmap(QPixmap(":/images/survivorIcon.png"));
            });

    connect(ui->speedsterButton, &QPushButton::clicked,
            this, [=]() {
                selectedTrait = "Speedster";
                ui->traitDescriptionLabel->setText("Move faster and escape traps more easily.");
                ui->traitIconLabel->setPixmap(QPixmap(":/images/speedsterIcon.png"));
            });


    // ═══════════════════════════════════════════════════
    // CONFIRM CHARACTER BUTTON
    // ═══════════════════════════════════════════════════

    connect(ui->confirmButton, &QPushButton::clicked,
            this, [=]() {

                playerName = ui->lineEdit->text();

                if (playerName.isEmpty()) {
                    ui->traitDescriptionLabel->setText("Enter your explorer name first.");
                    return;
                }

                // Update HUD with selected character
                ui->hudNameLabel->setText(playerName.toUpper());
                ui->hudTraitLabel->setText(selectedTrait.toUpper());

                if (selectedCharacter == "Zara")
                    ui->hudPortraitLabel->setPixmap(QPixmap(":/images/Zara.png"));
                else if (selectedCharacter == "Karim")
                    ui->hudPortraitLabel->setPixmap(QPixmap(":/images/Karim.png"));
                else if (selectedCharacter == "Nour")
                    ui->hudPortraitLabel->setPixmap(QPixmap(":/images/NOOR.png"));
                else if (selectedCharacter == "Rami")
                    ui->hudPortraitLabel->setPixmap(QPixmap(":/images/RAMI.png"));

                ui->stackedWidget->setCurrentWidget(ui->storyPage);
            });


    // ═══════════════════════════════════════════════════
    // ENTER TOMB BUTTON  — FIX: now calls startGame()
    // ═══════════════════════════════════════════════════

    connect(ui->enterTombButton, &QPushButton::clicked,
            this, [=]() {
                ui->stackedWidget->setCurrentWidget(ui->gamePage);
                startGame();   // ← was missing entirely before
            });


    // ═══════════════════════════════════════════════════
    // PAUSE MENU BUTTON
    // ═══════════════════════════════════════════════════

    connect(ui->pauseTestButton, &QPushButton::clicked,
            this, [=]() {

                if (!gameActive) return;
                loopTimer->stop();
                countdownTimer->stop();
                heldKeys.clear();

                PauseMenuDialog dialog(this);

                connect(&dialog, &PauseMenuDialog::resumeClicked,
                        this, [=]() {
                            if (gameActive) {
                                countdownTimer->start();
                                loopTimer->start();
                            }
                        });

                connect(&dialog, &PauseMenuDialog::restartClicked,
                        this, [=]() {
                            resetGame();
                            ui->stackedWidget->setCurrentWidget(ui->gamePage);
                            startGame();
                        });

                connect(&dialog, &PauseMenuDialog::mainMenuClicked,
                        this, [=]() {
                            resetGame();
                            ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);
                        });

                connect(&dialog, &PauseMenuDialog::exitClicked,
                        this, [=]() {
                            close();
                        });

                dialog.exec();

                // If dialog was closed without a signal (e.g. window X), resume
                if (gameActive && !countdownTimer->isActive()) {
                    countdownTimer->start();
                    loopTimer->start();
                }
            });


    // ═══════════════════════════════════════════════════
    // GAME OVER BUTTONS
    // ═══════════════════════════════════════════════════

    connect(ui->tryAgainButton, &QPushButton::clicked,
            this, [=]() {
                resetGame();
                ui->stackedWidget->setCurrentWidget(ui->gamePage);
                startGame();
            });

    connect(ui->returnMenuButton, &QPushButton::clicked,
            this, [=]() {
                resetGame();
                ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);
            });


    // ═══════════════════════════════════════════════════
    // WIN SCREEN BUTTONS
    // ═══════════════════════════════════════════════════

    connect(ui->playAgainButton, &QPushButton::clicked,
            this, [=]() {
                resetGame();
                ui->stackedWidget->setCurrentWidget(ui->gamePage);
                startGame();
            });

    connect(ui->leaveTempleButton, &QPushButton::clicked,
            this, [=]() {
                resetGame();
                ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);
            });


    // ═══════════════════════════════════════════════════
    // DIFFICULTY BUTTON  — FIX: now stores value, not just updates labels
    // ═══════════════════════════════════════════════════

    connect(ui->difficultyButton, &QPushButton::clicked,
            this, [=]() {

                DifficultyDialog dialog(this);

                connect(&dialog, &DifficultyDialog::difficultySelected,
                        this, [=](Difficulty difficulty) {

                            selectedDifficulty = difficulty;  // ← was never stored before

                            if (difficulty == Difficulty::EASY) {
                                ui->timerLabel->setText("12:00");
                                ui->livesLabel->setText("♥ ♥ ♥ ♥");
                            } else if (difficulty == Difficulty::MEDIUM) {
                                ui->timerLabel->setText("08:00");
                                ui->livesLabel->setText("♥ ♥ ♥");
                            } else if (difficulty == Difficulty::HARD) {
                                ui->timerLabel->setText("05:00");
                                ui->livesLabel->setText("♥ ♥");
                            }
                        });

                dialog.exec();
            });


    // ═══════════════════════════════════════════════════
    // HOW TO PLAY BUTTON
    // ═══════════════════════════════════════════════════

    connect(ui->howToPlayButton, &QPushButton::clicked,
            this, [=]() {
                HowToPlayDialog dialog(this);
                dialog.exec();
            });
}


// ═══════════════════════════════════════════════════════
// DESTRUCTOR
// ═══════════════════════════════════════════════════════

MainWindow::~MainWindow()
{
    delete ui;
}


// ═══════════════════════════════════════════════════════
// KEY EVENTS  — WASD to move, E to interact, ESC to pause
// ═══════════════════════════════════════════════════════

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    if (ui->stackedWidget->currentWidget() == ui->gamePage && gameActive) {
        switch (event->key()) {
        case Qt::Key_W: case Qt::Key_Up:    heldKeys.insert(Qt::Key_W); return;
        case Qt::Key_S: case Qt::Key_Down:  heldKeys.insert(Qt::Key_S); return;
        case Qt::Key_A: case Qt::Key_Left:  heldKeys.insert(Qt::Key_A); return;
        case Qt::Key_D: case Qt::Key_Right: heldKeys.insert(Qt::Key_D); return;
        case Qt::Key_E:   tryInteract();                                 return;
        case Qt::Key_Escape: ui->pauseTestButton->click();               return;
        default: break;
        }
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;
    switch (event->key()) {
    case Qt::Key_W: case Qt::Key_Up:    heldKeys.remove(Qt::Key_W); break;
    case Qt::Key_S: case Qt::Key_Down:  heldKeys.remove(Qt::Key_S); break;
    case Qt::Key_A: case Qt::Key_Left:  heldKeys.remove(Qt::Key_A); break;
    case Qt::Key_D: case Qt::Key_Right: heldKeys.remove(Qt::Key_D); break;
    default: break;
    }
    QMainWindow::keyReleaseEvent(event);
}


// ═══════════════════════════════════════════════════════
// COUNTDOWN TICK  — fires every 1 second
// ═══════════════════════════════════════════════════════

void MainWindow::onCountdownTick()
{
    if (timeSeconds > 0) timeSeconds--;
    refreshTimer();
    if (timeSeconds == 0) {
        countdownTimer->stop();
        triggerGameOver("Time has run out. The tomb has claimed you.");
    }
}


// ═══════════════════════════════════════════════════════
// GAME LOOP  — fires every ~16 ms (≈60 fps)
// ═══════════════════════════════════════════════════════

void MainWindow::onGameLoop()
{
    if (!gameActive || !playerSprite) return;

    // ── Move player ────────────────────────────────────
    float spd = (selectedTrait == "Speedster") ? 7.f : 5.f;
    float dx = 0, dy = 0;
    if (heldKeys.contains(Qt::Key_W)) dy -= spd;
    if (heldKeys.contains(Qt::Key_S)) dy += spd;
    if (heldKeys.contains(Qt::Key_A)) dx -= spd;
    if (heldKeys.contains(Qt::Key_D)) dx += spd;
    if (dx && dy) { dx *= 0.707f; dy *= 0.707f; }

    QPointF p = playerSprite->pos();
    playerSprite->setPos(
        qBound(0.f, float(p.x()) + dx, 836.f),  // 891 - 55
        qBound(0.f, float(p.y()) + dy, 346.f)   // 421 - 75
    );

    // ── Walk animation ────────────────────────────────
    bool isMoving = (dx != 0.f || dy != 0.f);
    if (isMoving) {
        if (++animTick >= 8) {
            animTick = 0;
            animFrame ^= 1;
            playerSprite->setPixmap(createPlayerSprite(animFrame));
        }
    } else if (animFrame != 0) {
        animFrame = 0;
        animTick  = 0;
        playerSprite->setPixmap(createPlayerSprite(0));
    }

    // ── Move traps ─────────────────────────────────────
    for (auto &t : traps) {
        QPointF tp = t.item->pos();
        if (t.horiz) {
            float nx = float(tp.x()) + t.spd * t.dir;
            if (nx <= t.mn) { nx = t.mn; t.dir = 1; }
            if (nx >= t.mx) { nx = t.mx; t.dir = -1; }
            t.item->setPos(nx, tp.y());
        } else {
            float ny = float(tp.y()) + t.spd * t.dir;
            if (ny <= t.mn) { ny = t.mn; t.dir = 1; }
            if (ny >= t.mx) { ny = t.mx; t.dir = -1; }
            t.item->setPos(tp.x(), ny);
        }
    }

    // ── Invincibility flash ────────────────────────────
    if (invincible) {
        invFrames--;
        playerSprite->setOpacity((invFrames / 5) % 2 == 0 ? 1.0 : 0.25);
        if (invFrames <= 0) {
            invincible = false;
            playerSprite->setOpacity(1.0);
        }
    } else {
        // ── Trap collision ──────────────────────────────
        QRectF pr = playerSprite->sceneBoundingRect();
        for (auto &t : traps) {
            if (pr.intersects(t.item->sceneBoundingRect())) {
                onTrapHit();
                break;
            }
        }
    }

    // ── Door proximity hint ────────────────────────────
    if (doorItem && pressEHint) {
        QPointF pc = playerSprite->sceneBoundingRect().center();
        QPointF dc = doorItem->sceneBoundingRect().center();
        float dist = qSqrt(qPow(pc.x()-dc.x(), 2) + qPow(pc.y()-dc.y(), 2));
        pressEHint->setVisible(dist < 130.f);
    }
}


// ═══════════════════════════════════════════════════════
// START GAME  — initialises everything; called from enterTombButton
// ═══════════════════════════════════════════════════════

void MainWindow::startGame()
{
    // Apply stored difficulty (diagnosis item 4)
    switch (selectedDifficulty) {
    case Difficulty::EASY: timeSeconds = 720; maxLives = 4; break;
    case Difficulty::HARD: timeSeconds = 300; maxLives = 2; break;
    default:               timeSeconds = 480; maxLives = 3; break;
    }

    lives      = maxLives;
    score      = 0;
    currentRoom = 0;
    gameActive  = true;
    invincible  = false;
    invFrames   = 0;
    riddleOpen  = false;
    heldKeys.clear();

    // Reset room-map sidebar
    for (int i = 0; i < 6; i++) mapLabels[i]->setStyleSheet(S_NORMAL);

    // Clear inventory
    ui->inventorySlot1->setPixmap(QPixmap()); ui->inventorySlot1->setText("");
    ui->inventorySlot2->setPixmap(QPixmap()); ui->inventorySlot2->setText("");
    ui->inventorySlot3->setPixmap(QPixmap()); ui->inventorySlot3->setText("");
    ui->inventorySlot4->setPixmap(QPixmap()); ui->inventorySlot4->setText("");

    refreshTimer();
    refreshLives();
    ui->scoreLabel->setText("🏆 0");

    setupScene();
    loadRoom(0);

    countdownTimer->start();
    loopTimer->start();
    this->setFocus();
}


// ═══════════════════════════════════════════════════════
// RESET GAME  — stops everything cleanly
// ═══════════════════════════════════════════════════════

void MainWindow::resetGame()
{
    loopTimer->stop();
    countdownTimer->stop();
    gameActive  = false;
    riddleOpen  = false;
    heldKeys.clear();

    clearRoom();

    if (playerSprite && scene) {
        scene->removeItem(playerSprite);
        delete playerSprite;
        playerSprite = nullptr;
    }
}


// ═══════════════════════════════════════════════════════
// SCENE SETUP  — creates the QGraphicsScene once
// ═══════════════════════════════════════════════════════

void MainWindow::setupScene()
{
    if (!scene) {
        scene = new QGraphicsScene(0, 0, 891, 421, this);
        scene->setSceneRect(0, 0, 891, 421);
        ui->graphicsView->setScene(scene);
    } else {
        clearRoom();
    }

    // (Re)create player sprite with selected character portrait
    if (playerSprite) {
        scene->removeItem(playerSprite);
        delete playerSprite;
        playerSprite = nullptr;
    }

    playerSprite = new QGraphicsPixmapItem(createPlayerSprite());
    playerSprite->setZValue(10);
    playerSprite->setPos(20, 173);
    scene->addItem(playerSprite);
}


// ═══════════════════════════════════════════════════════
// CLEAR ROOM  — removes all room objects (not the player)
// ═══════════════════════════════════════════════════════

void MainWindow::clearRoom()
{
    for (auto &t : traps) {
        if (scene) scene->removeItem(t.item);
        delete t.item;
    }
    traps.clear();

    for (QGraphicsItem *item : roomItems) {
        if (scene) scene->removeItem(item);
        delete item;
    }
    roomItems.clear();

    doorItem   = nullptr;
    pressEHint = nullptr;
}


// ═══════════════════════════════════════════════════════
// LOAD ROOM  — builds scene for one room, updates HUD
// ═══════════════════════════════════════════════════════

void MainWindow::loadRoom(int room)
{
    currentRoom = room;
    clearRoom();
    if (playerSprite) playerSprite->setPos(20, 173);

    // Background colour per room
    static const QColor BG[6] = {
        {45,32,12}, {28,22,18}, {12,4,28},
        {4,12,32},  {38,24,4},  {4,2,10}
    };
    scene->setBackgroundBrush(QBrush(BG[room]));

    // Floor strip
    addRoomRect(0, 360, 891, 61, QColor(0,0,0,80));

    // Room-specific atmosphere
    switch (room) {
    case 0:
        addRoomRect(140,40,30,310, QColor(110,85,45), QColor(150,120,70), 1.5, 1);
        addRoomRect(680,40,30,310, QColor(110,85,45), QColor(150,120,70), 1.5, 1);
        addRoomText("ENTRANCE", 330, 15, QColor(180,150,80,160), 18, 1);
        break;
    case 1:
        addRoomRect(200,20,20,340, QColor(70,60,50), QColor(100,90,80), 1, 1);
        addRoomRect(600,20,20,340, QColor(70,60,50), QColor(100,90,80), 1, 1);
        addRoomText("GUARDIAN HALL", 290, 340, QColor(160,130,70,180), 16, 1);
        break;
    case 2:
        for (int i = 0; i < 8; i++)
            addRoomRect(80+i*100, 30+(i%3)*80, 12, 12,
                        QColor(180,100,255, 100+i*18),
                        QColor(220,160,255,180), 1, 1);
        addRoomText("HALL OF ECHOES", 275, 340, QColor(180,100,255,180), 16, 1);
        break;
    case 3:
        for (int i = 0; i < 5; i++)
            addRoomRect(100+i*140,340,100,14, QColor(60,80,180,120),
                        QColor(100,140,255,180), 1, 1);
        addRoomText("TRIAL ROOM", 330, 15, QColor(100,140,255,180), 18, 1);
        break;
    case 4:
        addRoomRect(780,120,100,20, QColor(200,160,40,180), QColor(255,210,80), 2, 1);
        addRoomRect(800,80,60,42,   QColor(220,180,60,140), QColor(255,220,100), 2, 1);
        addRoomText("ANKH SANCTUARY", 265, 340, QColor(200,160,50,180), 16, 1);
        break;
    case 5:
        addRoomRect(760,0,131,420, QColor(15,8,30,200), QColor(80,40,120), 2, 1);
        addRoomRect(790,20,70,140, QColor(30,10,60,220), QColor(120,60,180), 2, 1);
        addRoomText("ANUBIS GATE", 310, 340, QColor(120,60,200,200), 18, 1);
        break;
    }

    // Golden door
    addRoomRect(793,152,79,116, QColor(255,200,50,40), QColor(255,200,50,100), 2, 2);
    doorItem = addRoomRect(800,160,65,100, QColor(180,130,20,200), QColor(255,210,60), 3, 3);
    addRoomText("ENTER", 808, 272, QColor(255,230,120), 13, 4);

    // Traps
    switch (room) {
    case 0: break; // intro — no traps
    case 1: addTrap(360,140,90,120, true,  160,680, 2.2f);         break;
    case 2: addTrap(280, 20,65, 90, false,  20,290, 2.5f);
            addTrap(560,100,65, 90, false, 100,330, 3.0f, -1.f);   break;
    case 3: addTrap(200,120,80, 70, true,  180,700, 2.8f);
            addTrap(200,250,80, 70, true,  180,700, 3.2f, -1.f);   break;
    case 4: addTrap(160,180,75, 75, true,  140,620, 2.5f);
            addTrap(430, 40,75, 75, false,  40,300, 2.8f);         break;
    case 5: addTrap(160, 80,70, 70, true,  140,730, 3.0f);
            addTrap(160,190,70, 70, true,  140,730, 3.6f, -1.f);
            addTrap(160,300,70, 70, true,  140,730, 2.8f);         break;
    }

    // "Press E" hint above door
    pressEHint = scene->addText("Press  E");
    pressEHint->setDefaultTextColor(QColor(255,255,120));
    QFont hf; hf.setPixelSize(14); hf.setBold(true);
    pressEHint->setFont(hf);
    pressEHint->setPos(794, 132);
    pressEHint->setZValue(20);
    pressEHint->setVisible(false);
    roomItems.append(pressEHint);

    // Update HUD
    ui->roomNameLabel->setText(ROOMS[room].name);
    setEventText(ROOMS[room].enterText);
    ui->levelLabel->setText(QString("%1 / 6").arg(room + 1));
    refreshMap();
}


// ═══════════════════════════════════════════════════════
// TRY INTERACT  — E key handler
// ═══════════════════════════════════════════════════════

void MainWindow::tryInteract()
{
    if (!gameActive || riddleOpen || !playerSprite || !doorItem) return;

    QPointF pc = playerSprite->sceneBoundingRect().center();
    QPointF dc = doorItem->sceneBoundingRect().center();
    float dist = qSqrt(qPow(pc.x()-dc.x(),2) + qPow(pc.y()-dc.y(),2));

    if (dist < 130.f) showRiddle();
}


// ═══════════════════════════════════════════════════════
// SHOW RIDDLE
// ═══════════════════════════════════════════════════════

void MainWindow::showRiddle()
{
    riddleOpen = true;
    loopTimer->stop();
    countdownTimer->stop();
    heldKeys.clear();

    RiddleDialog dlg(RIDDLES[currentRoom], selectedTrait, this);
    dlg.exec();

    if (!gameActive) return;   // reset() was called while dialog was open

    countdownTimer->start();
    loopTimer->start();
    riddleOpen = false;

    if (dlg.wasCorrect())
        onRiddleCorrect();
    else
        onRiddleWrong();
}


// ═══════════════════════════════════════════════════════
// RIDDLE CORRECT
// ═══════════════════════════════════════════════════════

void MainWindow::onRiddleCorrect()
{
    const RoomInfo &r = ROOMS[currentRoom];
    int earned = r.baseScore;

    if      (selectedTrait == "Scholar")   earned += r.bonusScore;
    else if (selectedTrait == "Survivor")  earned += r.bonusScore - 10;
    else if (selectedTrait == "Speedster") {
        earned += r.bonusScore - 10;
        timeSeconds += 30;
        refreshTimer();
    }

    float mult = 1.f;
    if (selectedDifficulty == Difficulty::MEDIUM) mult = 1.5f;
    else if (selectedDifficulty == Difficulty::HARD) mult = 2.f;

    score += int(earned * mult);
    ui->scoreLabel->setText(QString("🏆 %1").arg(score));

    // Survivor heals in Ankh room
    if (r.survivorHeals && selectedTrait == "Survivor" && lives < maxLives) {
        lives++;
        refreshLives();
    }

    setEventText(r.solvedText);

    if (currentRoom == 5)
        triggerWin();
    else
        loadRoom(currentRoom + 1);
}


// ═══════════════════════════════════════════════════════
// RIDDLE WRONG
// ═══════════════════════════════════════════════════════

void MainWindow::onRiddleWrong()
{
    lives--;
    refreshLives();

    if (lives <= 0) {
        triggerGameOver("You failed the ancient trial. The tomb claims you.");
        return;
    }

    if (playerSprite) playerSprite->setPos(20, 173);
    setEventText("Wrong! The tomb punishes you.\nWalk back to the door and press E to try again.");
}


// ═══════════════════════════════════════════════════════
// TRAP HIT
// ═══════════════════════════════════════════════════════

void MainWindow::onTrapHit()
{
    if (invincible) return;

    // Survivor: 50 % chance to resist
    if (selectedTrait == "Survivor" &&
        QRandomGenerator::global()->bounded(100) < 50) {
        invincible = true;
        invFrames  = 90;
        setEventText("The trap strikes — your endurance holds! (Survivor)");
        return;
    }

    lives--;
    refreshLives();
    invincible = true;
    invFrames  = 120;   // 2 s invincibility

    if (lives <= 0)
        triggerGameOver("The tomb's traps have consumed you.");
    else
        setEventText(QString("A trap strikes you! Lives remaining: %1").arg(lives));
}


// ═══════════════════════════════════════════════════════
// GAME OVER / WIN
// ═══════════════════════════════════════════════════════

void MainWindow::triggerGameOver(const QString &reason)
{
    loopTimer->stop();
    countdownTimer->stop();
    gameActive = false;

    ui->gameOverCauseLabel->setText(reason);
    ui->label_2->setText(QString::number(score));
    ui->stackedWidget->setCurrentWidget(ui->gameOverPage);
}

void MainWindow::triggerWin()
{
    loopTimer->stop();
    countdownTimer->stop();
    gameActive = false;

    score += timeSeconds;   // time-remaining bonus
    ui->winScoreLabel->setText(QString::number(score));
    ui->stackedWidget->setCurrentWidget(ui->winPage);
}


// ═══════════════════════════════════════════════════════
// SCENE HELPERS
// ═══════════════════════════════════════════════════════

QGraphicsRectItem* MainWindow::addRoomRect(qreal x, qreal y, qreal w, qreal h,
                                            QColor fill, QColor border,
                                            qreal bw, int z)
{
    QPen pen(border, bw);
    if (border == Qt::transparent) pen.setStyle(Qt::NoPen);
    auto *item = scene->addRect(x, y, w, h, pen, QBrush(fill));
    item->setZValue(z);
    roomItems.append(item);
    return item;
}

QGraphicsTextItem* MainWindow::addRoomText(const QString &txt, qreal x, qreal y,
                                             QColor col, int sz, int z)
{
    auto *item = scene->addText(txt);
    item->setDefaultTextColor(col);
    QFont f; f.setPixelSize(sz);
    item->setFont(f);
    item->setPos(x, y);
    item->setZValue(z);
    roomItems.append(item);
    return item;
}

void MainWindow::addTrap(qreal x, qreal y, qreal w, qreal h,
                          bool horiz, float mn, float mx, float spd, float startDir)
{
    auto *item = new QGraphicsRectItem(0, 0, w, h);
    item->setPen(QPen(QColor(255,80,50), 2.5));
    item->setBrush(QBrush(QColor(180,20,20,220)));
    item->setPos(x, y);
    item->setZValue(2);
    scene->addItem(item);

    // Warning "!" as child so it moves with the trap automatically
    auto *sym = new QGraphicsSimpleTextItem("!", item);
    QFont f("Arial", int(qMin(w,h) * 0.45), QFont::Bold);
    sym->setFont(f);
    sym->setBrush(QBrush(QColor(255,200,180)));
    sym->setPos(w/2 - sym->boundingRect().width()/2,
                h/2 - sym->boundingRect().height()/2);

    traps.append({item, horiz, mn, mx, spd, startDir});
}


// ═══════════════════════════════════════════════════════
// HUD HELPERS
// ═══════════════════════════════════════════════════════

void MainWindow::refreshTimer()
{
    int m = timeSeconds / 60, s = timeSeconds % 60;
    ui->timerLabel->setText(QString("%1:%2")
        .arg(m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0')));
}

void MainWindow::refreshLives()
{
    QString h;
    for (int i = 0; i < lives; i++) h += "♥ ";
    ui->livesLabel->setText(h.isEmpty() ? "—" : h.trimmed());
}

void MainWindow::refreshMap()
{
    for (int i = 0; i < 6; i++) {
        if      (i < currentRoom)  mapLabels[i]->setStyleSheet(S_DONE);
        else if (i == currentRoom) mapLabels[i]->setStyleSheet(S_ACTIVE);
        else                       mapLabels[i]->setStyleSheet(S_NORMAL);
    }
}

void MainWindow::setEventText(const QString &msg)
{
    ui->eventLabel->setText(msg);
}
