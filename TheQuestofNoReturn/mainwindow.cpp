#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config.h"
#include <QFileDialog>
#include <QLabel>
#include <QDebug>

// =====================================================
// CONSTRUCTOR
// =====================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // =====================================================
    // CREATE CORE OBJECTS
    // =====================================================

    m_player    = new Player(this);
    m_riddleGen = new RiddleGenerator(this);
    m_avatarGen = new AvatarGenerator(this);

    m_riddleGen->setApiKey(CLAUDE_API_KEY);
    m_avatarGen->setApiKey(CLAUDE_API_KEY);
    m_avatarGen->setOpenAiKey(OPENAI_API_KEY);

    // Wire player signals → UI updates (lives/score/timer labels)
    setupPlayerSignals();

    // Wire avatar generator signals
    connect(m_avatarGen, &AvatarGenerator::avatarReady,
            this, &MainWindow::onAvatarReady);

    connect(m_avatarGen, &AvatarGenerator::progressUpdate,
            this, [=](QString msg) {
                ui->descriptionLabel->setText(msg);
            });

    connect(m_avatarGen, &AvatarGenerator::generationFailed,
            this, [=]() {
                ui->descriptionLabel->setText(
                    "Photo generation failed — using default portrait.");
            });

    // =====================================================
    // START ON MAIN MENU
    // =====================================================

    ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);

    // =====================================================
    // PORTRAIT SETTINGS
    // =====================================================

    ui->portraitLabel->setScaledContents(true);
    ui->traitIconLabel->setScaledContents(true);

    // =====================================================
    // DEFAULT CHARACTER
    // =====================================================

    m_selectedCharacter = "Zara";

    ui->portraitLabel->setPixmap(QPixmap(":/images/Zara.png"));

    // =====================================================
    // DEFAULT TRAIT ICON
    // =====================================================

    ui->traitIconLabel->setPixmap(QPixmap(":/images/scholarIcon.png"));

    // =====================================================
    // DEFAULT TEXT
    // =====================================================

    ui->descriptionLabel->setText(
        "Fearless explorer driven by curiosity and courage.");

    ui->traitDescriptionLabel->setText(
        "Gain intelligence bonuses and uncover hidden clues.");

    // =====================================================
    // CHARACTER BUTTONS
    // =====================================================

    // ZARA
    connect(ui->zaraButton, &QPushButton::clicked, this, [=]() {
        m_selectedCharacter = "Zara";
        ui->portraitLabel->setPixmap(QPixmap(":/images/Zara.png"));
        ui->descriptionLabel->setText(
            "Fearless explorer driven by curiosity and courage.");
    });

    // KARIM
    connect(ui->karimButton, &QPushButton::clicked, this, [=]() {
        m_selectedCharacter = "Karim";
        ui->portraitLabel->setPixmap(QPixmap(":/images/Karim.png"));
        ui->descriptionLabel->setText(
            "Balanced adventurer skilled in combat and survival.");
    });

    // NOUR
    connect(ui->nourButton, &QPushButton::clicked, this, [=]() {
        m_selectedCharacter = "Nour";
        ui->portraitLabel->setPixmap(QPixmap(":/images/NOOR.png"));
        ui->descriptionLabel->setText(
            "Strategic thinker who uncovers hidden ancient secrets.");
    });

    // RAMI
    connect(ui->ramiButton, &QPushButton::clicked, this, [=]() {
        m_selectedCharacter = "Rami";
        ui->portraitLabel->setPixmap(QPixmap(":/images/RAMI.png"));
        ui->descriptionLabel->setText(
            "Fast and daring explorer who trusts instinct.");
    });

    // =====================================================
    // TRAIT BUTTONS
    // =====================================================

    // SCHOLAR
    connect(ui->scholarButton, &QPushButton::clicked, this, [=]() {
        m_selectedTrait = "Scholar";
        ui->traitDescriptionLabel->setText(
            "Gain intelligence bonuses and uncover hidden clues.");
        ui->traitIconLabel->setPixmap(QPixmap(":/images/scholarIcon.png"));
    });

    // SURVIVOR
    connect(ui->survivorButton, &QPushButton::clicked, this, [=]() {
        m_selectedTrait = "Survivor";
        ui->traitDescriptionLabel->setText(
            "Higher endurance and resistance to danger.");
        ui->traitIconLabel->setPixmap(QPixmap(":/images/survivorIcon.png"));
    });

    // SPEEDSTER
    connect(ui->speedsterButton, &QPushButton::clicked, this, [=]() {
        m_selectedTrait = "Speedster";
        ui->traitDescriptionLabel->setText(
            "Move faster and escape traps more easily.");
        ui->traitIconLabel->setPixmap(QPixmap(":/images/speedsterIcon.png"));
    });

    // =====================================================
    // CONFIRM BUTTON
    // =====================================================

    connect(ui->confirmButton, &QPushButton::clicked, this, [=]() {

        QString playerName = ui->lineEdit->text();

        // EMPTY NAME
        if (playerName.isEmpty()) {
            ui->traitDescriptionLabel->setText(
                "Enter your explorer name first.");
            return;
        }

        // NO CHARACTER
        if (m_selectedCharacter.isEmpty()) {
            ui->traitDescriptionLabel->setText(
                "Choose your explorer first.");
            return;
        }

        // NO TRAIT
        if (m_selectedTrait.isEmpty()) {
            ui->traitDescriptionLabel->setText(
                "Choose a trait before entering the tomb.");
            return;
        }

        // SHOW CONFIRMATION TEXT (same as before — this line is kept)
        ui->traitDescriptionLabel->setText(
            "Explorer " + playerName +
            " the " + m_selectedCharacter +
            " enters the tomb as a " + m_selectedTrait + ".");

        // NOW ALSO: configure the player object and start the game
        startGame();
    });

    // =====================================================
    // LOAD SAVE FILE IF ONE EXISTS
    // =====================================================

    // If a save exists from a previous session, load it silently.
    // The player will see their previous lives/score/timer when the game starts.
    if (SaveManager::hasSaveFile()) {
        int savedLevel = 1;
        SaveManager::loadGame(m_player, savedLevel);
        qDebug() << "Save file loaded. Level:" << savedLevel;
    }
}

// =====================================================
// DESTRUCTOR
// =====================================================

MainWindow::~MainWindow()
{
    // Save the game when the window closes (covers the "exit" case)
    if (m_player && m_player->getScore() > 0) {
        SaveManager::saveGame(m_player, 1);
    }
    delete ui;
}

// =====================================================
// START BUTTON  (navigates to character setup)
// =====================================================

void MainWindow::on_startButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->characterSetupScreen);
}

// =====================================================
// setupPlayerSignals
// Wires Player's signals to update the UI.
// Called once from the constructor after m_player is created.
// When Person 4 adds a real GameHUD, replace these with HUD calls.
// =====================================================

void MainWindow::setupPlayerSignals()
{
    // timerTick → update the timer display
    // (right now traitDescriptionLabel is used as a stand-in;
    //  Person 4 will replace this with their GameHUD::updateTimer slot)
    connect(m_player, &Player::timerTick, this, [=](int secondsLeft) {
        int mins = secondsLeft / 60;
        int secs = secondsLeft % 60;
        // Temporarily show timer in the description label while we have no HUD
        // Person 4's HUD slot replaces this connect
        qDebug() << "Timer:" << mins << "m" << secs << "s";
    });

    // livesChanged → debug for now; Person 4 HUD handles display
    connect(m_player, &Player::livesChanged, this, [=](int lives) {
        qDebug() << "Lives:" << lives;
    });

    // scoreChanged → debug for now; Person 4 HUD handles display
    connect(m_player, &Player::scoreChanged, this, [=](int score) {
        qDebug() << "Score:" << score;
    });

    // gameOver → show game over screen
    connect(m_player, &Player::gameOver, this, &MainWindow::onGameOver);

    // timerExpired also ends the game
    connect(m_player, &Player::timerExpired, this, &MainWindow::onGameOver);
}

// =====================================================
// startGame
// Called by the confirm button after validation passes.
// Reads the selected character/trait, configures the
// Player object, and transitions to the story/game page.
// =====================================================

void MainWindow::startGame()
{
    // Map the selected strings to enum values
    CharacterID charID =
        (m_selectedCharacter == "Zara")  ? CharacterID::ZARA  :
            (m_selectedCharacter == "Karim") ? CharacterID::KARIM :
            (m_selectedCharacter == "Nour")  ? CharacterID::NOUR  : CharacterID::RAMI;

    CharacterTrait trait =
        (m_selectedTrait == "Scholar")  ? CharacterTrait::SCHOLAR  :
            (m_selectedTrait == "Survivor") ? CharacterTrait::SURVIVOR : CharacterTrait::SPEEDSTER;

    // Configure the player
    m_player->setCharacterName(ui->lineEdit->text());
    m_player->setCharacterID(charID);
    m_player->setTrait(trait);
    m_player->setDifficulty(m_selectedDifficulty);  // default MEDIUM

    // reset() applies difficulty defaults (lives, timer) + trait bonuses
    m_player->reset();

    // Start the countdown timer
    m_player->startTimer();

    // Navigate to the game/story page
    // Person 1 will replace storyPage with their actual game scene page
    ui->stackedWidget->setCurrentWidget(ui->storyPage);

    qDebug() << "Game started:"
             << m_player->getCharacterName()
             << "| Lives:" << m_player->getLives()
             << "| Time:" << m_player->getSecondsRemaining();
}

// =====================================================
// onGameOver
// Triggered by Player::gameOver or Player::timerExpired.
// Saves the game state, then shows the game over screen.
// =====================================================

void MainWindow::onGameOver()
{
    // Stop the timer
    m_player->pauseTimer();

    // Save current state to file (covers the game-over case)
    SaveManager::saveGame(m_player, 1);

    // Navigate to game over screen.
    // Person 4 will add a proper gameOverScreen page to the stacked widget.
    // For now, go back to main menu as a stand-in.
    ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);

    qDebug() << "Game Over. Final score:" << m_player->getScore();
}

// =====================================================
// onAvatarReady
// Called when AvatarGenerator finishes generating the portrait.
// Updates the portrait label in the character setup screen.
// =====================================================

void MainWindow::onAvatarReady(QString path)
{
    m_player->setAvatarPath(path);

    QPixmap avatar(path);
    if (!avatar.isNull()) {
        ui->portraitLabel->setPixmap(
            avatar.scaled(ui->portraitLabel->size(),
                          Qt::KeepAspectRatio,
                          Qt::SmoothTransformation));
    }

    ui->descriptionLabel->setText("Portrait ready! Confirm to enter the tomb.");
}
