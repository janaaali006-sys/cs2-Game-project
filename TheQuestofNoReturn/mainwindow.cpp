#include "mainwindow.h"
#include "RoomManager.h"
#include "Room.h"
#include "ui_mainwindow.h"
#include "config.h"
#include "pausemenudialog.h"
#include "difficultydialog.h"
#include "howtoplaydialog.h"
#include "player.h"
#include <QFileDialog>
#include <QGraphicsView>
#include <QDebug>

// =====================================================
// CONSTRUCTOR
// =====================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ---- Find the gameView widget Person 4 added to mainwindow.ui ----
    QGraphicsView* gameView = findChild<QGraphicsView*>("gameView");
    if (!gameView) {
        gameView = new QGraphicsView(this);
        gameView->hide();
    }

    // ---- Core game object ----
    m_game = new Game(gameView, this);
    connect(m_game, &Game::gameEnded, this, &MainWindow::onGameEnded);

    // ---- Avatar generator ----
    setupAvatarGenerator();

    // ---- Start on main menu ----
    ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);

    // ---- Portrait / icon labels ----
    ui->portraitLabel->setScaledContents(true);
    ui->traitIconLabel->setScaledContents(true);
    ui->hudPortraitLabel->setScaledContents(true);
    ui->inventorySlot1->setScaledContents(true);
    ui->inventorySlot2->setScaledContents(true);
    ui->inventorySlot3->setScaledContents(true);
    ui->inventorySlot4->setScaledContents(true);

    // ---- Character setup defaults ----
    ui->portraitLabel->setPixmap(QPixmap(":/images/Zara.png"));
    ui->traitIconLabel->setPixmap(QPixmap(":/images/scholarIcon.png"));
    ui->descriptionLabel->setText("Fearless explorer driven by curiosity and courage.");
    ui->traitDescriptionLabel->setText("Gain intelligence bonuses and uncover hidden clues.");

    // ---- HUD defaults ----
    updateHUDDefaults();

    // ---- Wire all buttons ----
    setupMenuButtons();
    setupCharacterButtons();
    setupConfirmButton();
    setupHUDButtons();

    // ---- Load save if exists ----
    if (SaveManager::hasSaveFile()) {
        int savedLevel = 1;
        SaveManager::loadGame(m_game->player(), savedLevel);
        qDebug() << "Save loaded. Level:" << savedLevel;
    }
}

// =====================================================
// DESTRUCTOR
// =====================================================

MainWindow::~MainWindow()
{
    if (m_game && m_game->player() && m_game->player()->getScore() > 0)
        SaveManager::saveGame(m_game->player(), m_game->currentLevel());
    delete ui;
}

// =====================================================
// HUD DEFAULTS
// =====================================================

void MainWindow::updateHUDDefaults()
{
    ui->hudPortraitLabel->setPixmap(QPixmap(":/images/Zara.png"));
    ui->hudNameLabel->setText("ZARA");
    ui->hudTraitLabel->setText("SCHOLAR");
    ui->livesLabel->setText("♥ ♥");
    ui->timerLabel->setText("08:00");
    ui->scoreLabel->setText("🏆 0");
    ui->levelLabel->setText("1 / 6");
    ui->roomNameLabel->setText("Temple Entrance");
    ui->eventLabel->setText("A cold wind echoes through the chamber...");
    ui->controlsLabel->setText("Arrow Keys: Move  |  ESC: Pause");
    ui->inventorySlot1->setPixmap(QPixmap(":/images/emptySlot.png"));
    ui->inventorySlot2->setPixmap(QPixmap(":/images/emptySlot.png"));
    ui->inventorySlot3->setPixmap(QPixmap(":/images/emptySlot.png"));
    ui->inventorySlot4->setPixmap(QPixmap(":/images/emptySlot.png"));
}

// =====================================================
// MAIN MENU BUTTONS
// =====================================================

void MainWindow::setupMenuButtons()
{
    // START → character setup
    connect(ui->startButton, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->characterSetupScreen);
    });

    // EXIT
    connect(ui->exitButton, &QPushButton::clicked, this, [=]() { close(); });

    // DIFFICULTY — opens DifficultyDialog, receives chosen difficulty
    connect(ui->difficultyButton, &QPushButton::clicked, this, [=]() {
        DifficultyDialog dialog(this);
        connect(&dialog, &DifficultyDialog::difficultySelected,
                this, [=](Difficulty d) {
                    m_selectedDifficulty = d;
                    // Update HUD timer/lives preview
                    if (d == Difficulty::EASY) {
                        ui->timerLabel->setText("12:00");
                        ui->livesLabel->setText("♥ ♥ ♥ ♥");
                    } else if (d == Difficulty::MEDIUM) {
                        ui->timerLabel->setText("08:00");
                        ui->livesLabel->setText("♥ ♥ ♥");
                    } else {
                        ui->timerLabel->setText("05:00");
                        ui->livesLabel->setText("♥ ♥");
                    }
                });
        dialog.exec();
    });

    // HOW TO PLAY
    connect(ui->howToPlayButton, &QPushButton::clicked, this, [=]() {
        HowToPlayDialog dialog(this);
        dialog.exec();
    });

    // STORY PAGE → enter tomb button
    connect(ui->enterTombButton, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->gamePage);
    });

    // GAME OVER → try again
    connect(ui->tryAgainButton, &QPushButton::clicked, this, [=]() {
        m_game->restartGame();
        setupPlayerHUDSignals();
        ui->stackedWidget->setCurrentWidget(ui->gamePage);
    });

    // GAME OVER → main menu
    connect(ui->returnMenuButton, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);
    });

    // WIN → play again
    connect(ui->playAgainButton, &QPushButton::clicked, this, [=]() {
        m_game->restartGame();
        setupPlayerHUDSignals();
        ui->stackedWidget->setCurrentWidget(ui->gamePage);
    });

    // WIN → leave temple
    connect(ui->leaveTempleButton, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);
    });
}

// =====================================================
// HUD BUTTONS (pause)
// =====================================================

void MainWindow::setupHUDButtons()
{
    connect(ui->pauseTestButton, &QPushButton::clicked, this, [=]() {
        m_game->pauseGame();

        PauseMenuDialog dialog(this);

        connect(&dialog, &PauseMenuDialog::resumeClicked, this, [=]() {
            m_game->resumeGame();
            ui->stackedWidget->setCurrentWidget(ui->gamePage);
        });

        connect(&dialog, &PauseMenuDialog::restartClicked, this, [=]() {
            m_game->restartGame();
            setupPlayerHUDSignals();
            ui->stackedWidget->setCurrentWidget(ui->gamePage);
        });

        connect(&dialog, &PauseMenuDialog::mainMenuClicked, this, [=]() {
            ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);
        });

        connect(&dialog, &PauseMenuDialog::exitClicked, this, [=]() {
            close();
        });

        dialog.exec();
    });
}

// =====================================================
// CHARACTER BUTTONS
// =====================================================

void MainWindow::setupCharacterButtons()
{
    connect(ui->zaraButton, &QPushButton::clicked, this, [=]() {
        m_selectedCharacter = "Zara";
        ui->portraitLabel->setPixmap(QPixmap(":/images/Zara.png"));
        ui->descriptionLabel->setText("Fearless explorer driven by curiosity and courage.");
    });

    connect(ui->karimButton, &QPushButton::clicked, this, [=]() {
        m_selectedCharacter = "Karim";
        ui->portraitLabel->setPixmap(QPixmap(":/images/Karim.png"));
        ui->descriptionLabel->setText("Balanced adventurer skilled in combat and survival.");
    });

    connect(ui->nourButton, &QPushButton::clicked, this, [=]() {
        m_selectedCharacter = "Nour";
        ui->portraitLabel->setPixmap(QPixmap(":/images/NOOR.png"));
        ui->descriptionLabel->setText("Strategic thinker who uncovers hidden ancient secrets.");
    });

    connect(ui->ramiButton, &QPushButton::clicked, this, [=]() {
        m_selectedCharacter = "Rami";
        ui->portraitLabel->setPixmap(QPixmap(":/images/RAMI.png"));
        ui->descriptionLabel->setText("Fast and daring explorer who trusts instinct.");
    });

    connect(ui->scholarButton, &QPushButton::clicked, this, [=]() {
        m_selectedTrait = "Scholar";
        ui->traitDescriptionLabel->setText("Gain intelligence bonuses and uncover hidden clues.");
        ui->traitIconLabel->setPixmap(QPixmap(":/images/scholarIcon.png"));
    });

    connect(ui->survivorButton, &QPushButton::clicked, this, [=]() {
        m_selectedTrait = "Survivor";
        ui->traitDescriptionLabel->setText("Higher endurance and resistance to danger.");
        ui->traitIconLabel->setPixmap(QPixmap(":/images/survivorIcon.png"));
    });

    connect(ui->speedsterButton, &QPushButton::clicked, this, [=]() {
        m_selectedTrait = "Speedster";
        ui->traitDescriptionLabel->setText("Move faster and escape traps more easily.");
        ui->traitIconLabel->setPixmap(QPixmap(":/images/speedsterIcon.png"));
    });
}

// =====================================================
// CONFIRM BUTTON
// =====================================================

void MainWindow::setupConfirmButton()
{
    connect(ui->confirmButton, &QPushButton::clicked, this, [=]() {

        QString playerName = ui->lineEdit->text();

        if (playerName.isEmpty()) {
            ui->traitDescriptionLabel->setText("Enter your explorer name first.");
            return;
        }
        if (m_selectedCharacter.isEmpty()) {
            ui->traitDescriptionLabel->setText("Choose your explorer first.");
            return;
        }
        if (m_selectedTrait.isEmpty()) {
            ui->traitDescriptionLabel->setText("Choose a trait before entering the tomb.");
            return;
        }

        ui->traitDescriptionLabel->setText(
            "Explorer " + playerName + " the " + m_selectedCharacter +
            " enters the tomb as a " + m_selectedTrait + ".");

        // Update HUD portrait + name
        ui->hudNameLabel->setText(playerName.toUpper());
        ui->hudTraitLabel->setText(m_selectedTrait.toUpper());
        if (m_selectedCharacter == "Zara")
            ui->hudPortraitLabel->setPixmap(QPixmap(":/images/Zara.png"));
        else if (m_selectedCharacter == "Karim")
            ui->hudPortraitLabel->setPixmap(QPixmap(":/images/Karim.png"));
        else if (m_selectedCharacter == "Nour")
            ui->hudPortraitLabel->setPixmap(QPixmap(":/images/NOOR.png"));
        else
            ui->hudPortraitLabel->setPixmap(QPixmap(":/images/RAMI.png"));

        // Go to story page first (then enterTombButton starts the game)
        ui->stackedWidget->setCurrentWidget(ui->storyPage);

        // Pre-configure game (actual start happens on enterTombButton)
        startGame();
    });
}

// =====================================================
// startGame
// =====================================================

void MainWindow::startGame()
{
    CharacterID charID =
        (m_selectedCharacter == "Zara")  ? CharacterID::ZARA  :
        (m_selectedCharacter == "Karim") ? CharacterID::KARIM :
        (m_selectedCharacter == "Nour")  ? CharacterID::NOUR  : CharacterID::RAMI;

    CharacterTrait trait =
        (m_selectedTrait == "Scholar")  ? CharacterTrait::SCHOLAR  :
        (m_selectedTrait == "Survivor") ? CharacterTrait::SURVIVOR : CharacterTrait::SPEEDSTER;

    QString avatarPath = m_game->player() ? m_game->player()->getAvatarPath() : "";

    m_game->startNewGame(m_selectedDifficulty, trait, charID,
                         ui->lineEdit->text(), avatarPath);

    setupPlayerHUDSignals();
}

// =====================================================
// setupPlayerHUDSignals
// Wires Player signals → Person 4's HUD labels
// =====================================================

void MainWindow::setupPlayerHUDSignals()
{
    Player* p = m_game->player();
    if (!p) return;

    // Timer → timerLabel
    connect(p, &Player::timerTick, this, [=](int secondsLeft) {
        int mins = secondsLeft / 60, secs = secondsLeft % 60;
        ui->timerLabel->setText(
            QString("%1:%2").arg(mins).arg(secs, 2, 10, QChar('0')));
    });

    // Lives → livesLabel (hearts)
    connect(p, &Player::livesChanged, this, [=](int lives) {
        QString hearts = "";
        for (int i = 0; i < lives; i++) hearts += "♥ ";
        ui->livesLabel->setText(hearts.trimmed());
    });

    // Score → scoreLabel
    connect(p, &Player::scoreChanged, this, [=](int score) {
        ui->scoreLabel->setText("🏆 " + QString::number(score));
    });

    // Level → levelLabel (updated from RoomManager via Game)
    connect(m_game, &Game::stateChanged, this, [=](GameState) {
        if (m_game->roomManager()) {
            int lvl = m_game->currentLevel();
            ui->levelLabel->setText(QString("%1 / 6").arg(lvl));
            // Update room name if available
            if (m_game->roomManager()->currentRoom()) {
                ui->roomNameLabel->setText(
                    m_game->roomManager()->currentRoom()->chamberName());
            }
        }
    });
}

// =====================================================
// onGameEnded — routes to correct screen
// =====================================================

void MainWindow::onGameEnded(bool won)
{
    if (won) {
        // TODO: replace with actual win page name from mainwindow.ui
        ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);
    } else {
        // TODO: replace with actual game over page name from mainwindow.ui
        ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);
    }
}

// =====================================================
// setupAvatarGenerator
// =====================================================

void MainWindow::setupAvatarGenerator()
{
    m_avatarGen = new AvatarGenerator(this);
    m_avatarGen->setApiKey(GOOGLEAI_API_KEY);
    m_avatarGen->setOpenAiKey(OPENAI_API_KEY);

    connect(m_avatarGen, &AvatarGenerator::avatarReady,
            this, &MainWindow::onAvatarReady);

    connect(m_avatarGen, &AvatarGenerator::progressUpdate,
            this, [=](QString msg) { ui->descriptionLabel->setText(msg); });

    connect(m_avatarGen, &AvatarGenerator::generationFailed, this, [=]() {
        ui->descriptionLabel->setText("Photo generation failed — using default portrait.");
    });

    QPushButton* uploadBtn = findChild<QPushButton*>("uploadButton");
    if (uploadBtn) {
        connect(uploadBtn, &QPushButton::clicked, this, [=]() {
            QString path = QFileDialog::getOpenFileName(
                this, "Select Photo", "", "Images (*.png *.jpg *.jpeg)");
            if (!path.isEmpty())
                m_avatarGen->generateFromPhoto(path);
        });
    }
}

// =====================================================
// onAvatarReady
// =====================================================

void MainWindow::onAvatarReady(QString path)
{
    if (m_game->player()) m_game->player()->setAvatarPath(path);
    QPixmap avatar(path);
    if (!avatar.isNull()) {
        ui->portraitLabel->setPixmap(
            avatar.scaled(ui->portraitLabel->size(),
                          Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    ui->descriptionLabel->setText("Portrait ready! Confirm to enter the tomb.");
}
