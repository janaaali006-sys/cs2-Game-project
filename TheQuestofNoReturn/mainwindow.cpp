#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config.h"
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

    // ---- Find (or create) a QGraphicsView for the game scene ----
    // Person 4 will add a proper gameView widget; we look for it by name.
    // If not found yet, we create a temporary one.
    QGraphicsView* gameView = findChild<QGraphicsView*>("gameView");
    if (!gameView) {
        gameView = new QGraphicsView(this);  // temporary until Person 4 adds it to .ui
        gameView->hide();
    }

    // ---- Core game object ----
    m_game = new Game(gameView, this);
    connect(m_game, &Game::gameEnded, this, &MainWindow::onGameEnded);

    // ---- Avatar generator ----
    setupAvatarGenerator();

    // ---- UI defaults ----
    ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);
    ui->portraitLabel->setScaledContents(true);
    ui->traitIconLabel->setScaledContents(true);

    ui->portraitLabel->setPixmap(QPixmap(":/images/Zara.png"));
    ui->traitIconLabel->setPixmap(QPixmap(":/images/scholarIcon.png"));
    ui->descriptionLabel->setText("Fearless explorer driven by curiosity and courage.");
    ui->traitDescriptionLabel->setText("Gain intelligence bonuses and uncover hidden clues.");

    // ---- Wire all buttons ----
    setupCharacterButtons();
    setupConfirmButton();

    // ---- Load save file if one exists ----
    if (SaveManager::hasSaveFile()) {
        int savedLevel = 1;
        SaveManager::loadGame(m_game->player(), savedLevel);
        qDebug() << "Save loaded. Level:" << savedLevel;
    }
}

// =====================================================
// DESTRUCTOR — save on exit
// =====================================================

MainWindow::~MainWindow()
{
    if (m_game && m_game->player() && m_game->player()->getScore() > 0) {
        SaveManager::saveGame(m_game->player(), m_game->currentLevel());
    }
    delete ui;
}

// =====================================================
// START BUTTON — go to character setup
// =====================================================

void MainWindow::on_startButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->characterSetupScreen);
}

// =====================================================
// CHARACTER + TRAIT BUTTONS
// =====================================================

void MainWindow::setupCharacterButtons()
{
    // ZARA
    connect(ui->zaraButton, &QPushButton::clicked, this, [=]() {
        m_selectedCharacter = "Zara";
        ui->portraitLabel->setPixmap(QPixmap(":/images/Zara.png"));
        ui->descriptionLabel->setText("Fearless explorer driven by curiosity and courage.");
    });

    // KARIM
    connect(ui->karimButton, &QPushButton::clicked, this, [=]() {
        m_selectedCharacter = "Karim";
        ui->portraitLabel->setPixmap(QPixmap(":/images/Karim.png"));
        ui->descriptionLabel->setText("Balanced adventurer skilled in combat and survival.");
    });

    // NOUR
    connect(ui->nourButton, &QPushButton::clicked, this, [=]() {
        m_selectedCharacter = "Nour";
        ui->portraitLabel->setPixmap(QPixmap(":/images/NOOR.png"));
        ui->descriptionLabel->setText("Strategic thinker who uncovers hidden ancient secrets.");
    });

    // RAMI
    connect(ui->ramiButton, &QPushButton::clicked, this, [=]() {
        m_selectedCharacter = "Rami";
        ui->portraitLabel->setPixmap(QPixmap(":/images/RAMI.png"));
        ui->descriptionLabel->setText("Fast and daring explorer who trusts instinct.");
    });

    // SCHOLAR
    connect(ui->scholarButton, &QPushButton::clicked, this, [=]() {
        m_selectedTrait = "Scholar";
        ui->traitDescriptionLabel->setText("Gain intelligence bonuses and uncover hidden clues.");
        ui->traitIconLabel->setPixmap(QPixmap(":/images/scholarIcon.png"));
    });

    // SURVIVOR
    connect(ui->survivorButton, &QPushButton::clicked, this, [=]() {
        m_selectedTrait = "Survivor";
        ui->traitDescriptionLabel->setText("Higher endurance and resistance to danger.");
        ui->traitIconLabel->setPixmap(QPixmap(":/images/survivorIcon.png"));
    });

    // SPEEDSTER
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

        // EMPTY NAME
        if (playerName.isEmpty()) {
            ui->traitDescriptionLabel->setText("Enter your explorer name first.");
            return;
        }

        // NO CHARACTER
        if (m_selectedCharacter.isEmpty()) {
            ui->traitDescriptionLabel->setText("Choose your explorer first.");
            return;
        }

        // NO TRAIT
        if (m_selectedTrait.isEmpty()) {
            ui->traitDescriptionLabel->setText("Choose a trait before entering the tomb.");
            return;
        }

        // SHOW CONFIRMATION TEXT (same as before)
        ui->traitDescriptionLabel->setText(
            "Explorer " + playerName +
            " the " + m_selectedCharacter +
            " enters the tomb as a " + m_selectedTrait + ".");

        // START THE GAME
        startGame();
    });
}

// =====================================================
// startGame — configure and launch via Game object
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

    // Game::startNewGame creates/resets Player + RoomManager and starts timer
    m_game->startNewGame(m_selectedDifficulty, trait, charID,
                         ui->lineEdit->text(), avatarPath);

    // Wire HUD signals AFTER player is created inside Game
    setupPlayerHUDSignals();

    // Navigate to game page (storyPage is stand-in until Person 4 adds gameScreen)
    ui->stackedWidget->setCurrentWidget(ui->storyPage);
}

// =====================================================
// setupPlayerHUDSignals
// Called after startNewGame() so m_game->player() exists.
// Connect Player signals → UI updates.
// Person 4 will replace the qDebug lines with their HUD slots.
// =====================================================

void MainWindow::setupPlayerHUDSignals()
{
    Player* p = m_game->player();
    if (!p) return;

    connect(p, &Player::timerTick, this, [=](int secondsLeft) {
        // TODO Person 4: replace with hud->updateTimer(secondsLeft)
        int mins = secondsLeft / 60, secs = secondsLeft % 60;
        qDebug() << QString("Timer: %1:%2").arg(mins).arg(secs, 2, 10, QChar('0'));
    });

    connect(p, &Player::livesChanged, this, [=](int lives) {
        // TODO Person 4: replace with hud->updateLives(lives)
        qDebug() << "Lives:" << lives;
    });

    connect(p, &Player::scoreChanged, this, [=](int score) {
        // TODO Person 4: replace with hud->updateScore(score)
        qDebug() << "Score:" << score;
    });
}

// =====================================================
// onGameEnded — called by Game::gameEnded signal
// =====================================================

void MainWindow::onGameEnded(bool won)
{
    if (won) {
        // TODO Person 4: setCurrentWidget(ui->winScreen)
        qDebug() << "YOU WIN! Score:" << m_game->player()->getScore();
        ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);
    } else {
        // TODO Person 4: setCurrentWidget(ui->gameOverScreen)
        qDebug() << "GAME OVER. Score:" << m_game->player()->getScore();
        ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);
    }
}

// =====================================================
// setupAvatarGenerator
// =====================================================

void MainWindow::setupAvatarGenerator()
{
    m_avatarGen = new AvatarGenerator(this);
    m_avatarGen->setApiKey(CLAUDE_API_KEY);
    m_avatarGen->setOpenAiKey(OPENAI_API_KEY);

    connect(m_avatarGen, &AvatarGenerator::avatarReady,
            this, &MainWindow::onAvatarReady);

    connect(m_avatarGen, &AvatarGenerator::progressUpdate,
            this, [=](QString msg) {
                ui->descriptionLabel->setText(msg);
            });

    connect(m_avatarGen, &AvatarGenerator::generationFailed, this, [=]() {
        ui->descriptionLabel->setText("Photo generation failed — using default portrait.");
    });

    // Wire upload button if Person 4 has added it to the UI
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
    if (m_game->player())
        m_game->player()->setAvatarPath(path);

    QPixmap avatar(path);
    if (!avatar.isNull()) {
        ui->portraitLabel->setPixmap(
            avatar.scaled(ui->portraitLabel->size(),
                          Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    ui->descriptionLabel->setText("Portrait ready! Confirm to enter the tomb.");
}
