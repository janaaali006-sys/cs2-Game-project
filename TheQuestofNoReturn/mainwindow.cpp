#include "mainwindow.h"
#include "RoomManager.h"
#include "Room.h"
#include "ui_mainwindow.h"
#include "config.h"
#include "pausemenudialog.h"
#include "difficultydialog.h"
#include "howtoplaydialog.h"
#include "Artefact.h"
#include "player.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsView>
#include <QPainter>
#include <QDebug>
#include <QPushButton>
#include <QResizeEvent>
#include <QStringList>

static QPixmap loadGamePixmap(const QString& resourceOrFilePath)
{
    QPixmap px(resourceOrFilePath);
    if (!px.isNull())
        return px;

    if (resourceOrFilePath.startsWith(":/images/")) {
        const QString baseName = resourceOrFilePath.mid(QString(":/images/").size());
        const QString sourceDir = QFileInfo(QString::fromUtf8(__FILE__)).absolutePath();
        px.load(sourceDir + "/" + baseName);
    }
    return px;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    applyPageBackgrounds();

    // ---- Find graphicsView (Person 4's actual widget name in mainwindow.ui) ----
    QGraphicsView* gameView = findChild<QGraphicsView*>("graphicsView");
    if (!gameView) {
        // Fallback: create one and add it to gamePage so it's visible
        gameView = new QGraphicsView(ui->gamePage);
        gameView->setGeometry(390, 120, 891, 480);
    }
    gameView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gameView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gameView->setFocusPolicy(Qt::StrongFocus);
    gameView->setAlignment(Qt::AlignCenter);
    gameView->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    gameView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    gameView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    gameView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // ---- Core game object ----
    m_game = new Game(gameView, this);
    connect(m_game, &Game::gameEnded, this, &MainWindow::onGameEnded);
    connect(m_game, &Game::stateChanged, this, [this](GameState state) {
        setUiState(state);
    });

    // ---- Avatar generator ----
    setupAvatarGenerator();

    // ---- Start on main menu ----
    setUiState(GameState::MENU);
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, [this](int) {
        applyPageBackgrounds();
    });

    const QStringList missingAssets = validateCriticalAssets();
    if (!missingAssets.isEmpty()) {
        ui->eventLabel->setText(
            QString("Missing assets detected: %1").arg(missingAssets.join(", ")));
    }
    layoutGamePage();

    // ---- Portrait / icon labels ----
    ui->portraitLabel->setScaledContents(true);
    ui->traitIconLabel->setScaledContents(true);
    ui->hudPortraitLabel->setScaledContents(true);
    ui->inventorySlot1->setScaledContents(true);
    ui->inventorySlot2->setScaledContents(true);
    ui->inventorySlot3->setScaledContents(true);
    ui->inventorySlot4->setScaledContents(true);
    ui->inventorySlot1->setFixedSize(64, 44);
    ui->inventorySlot2->setFixedSize(64, 44);
    ui->inventorySlot3->setFixedSize(64, 44);
    ui->inventorySlot4->setFixedSize(64, 44);

    // ---- Character setup defaults ----
    ui->portraitLabel->setPixmap(loadGamePixmap(":/images/Zara.png"));
    ui->traitIconLabel->setPixmap(loadGamePixmap(":/images/scholarIcon.png"));
    ui->winBackgroundLabel->setPixmap(loadGamePixmap(":/images/winBackground.png"));
    ui->gameOverBackground->setPixmap(loadGamePixmap(":/images/gameover_background.png"));
    ui->storyBackgroundLabel->setPixmap(loadGamePixmap(":/images/storyBackground.png"));
    ui->gameBackgroundLabel->setPixmap(loadGamePixmap(":/images/gameBackground.png"));
    ui->storyBackgroundLabel->setScaledContents(true);
    ui->gameBackgroundLabel->setScaledContents(true);
    ui->gameOverBackground->setScaledContents(true);
    ui->winBackgroundLabel->setScaledContents(true);
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

void MainWindow::applyPageBackgrounds()
{
    // Make backgrounds robust even if Designer stylesheet is overridden.
    const QString stretch = "0 0 0 0 stretch stretch";
    auto applyBgPalette = [](QWidget* w, const QPixmap& px) {
        if (!w || px.isNull()) return;
        QPalette pal = w->palette();
        pal.setBrush(QPalette::Window, QBrush(px.scaled(w->size(),
                                                        Qt::KeepAspectRatioByExpanding,
                                                        Qt::SmoothTransformation)));
        w->setAutoFillBackground(true);
        w->setPalette(pal);
    };

    applyBgPalette(ui->mainMenuPage, loadGamePixmap(":/images/background.png"));
    applyBgPalette(ui->storyPage, loadGamePixmap(":/images/storyBackground.png"));
    applyBgPalette(ui->gamePage, loadGamePixmap(":/images/gameBackground.png"));
    applyBgPalette(ui->gameOverPage, loadGamePixmap(":/images/gameover_background.png"));
    applyBgPalette(ui->winPage, loadGamePixmap(":/images/winBackground.png"));
    applyBgPalette(ui->characterSetupScreen, loadGamePixmap(":/images/characterPage.png"));

    // Ensure all dedicated background widgets fully cover their pages.
    if (ui->backgroundFrame) {
        ui->backgroundFrame->setGeometry(ui->mainMenuPage->rect());
    }
    if (ui->storyBackgroundLabel) {
        ui->storyBackgroundLabel->setGeometry(ui->storyPage->rect());
    }
    if (ui->gameBackgroundLabel) {
        ui->gameBackgroundLabel->setGeometry(ui->gamePage->rect());
    }
    if (ui->gameOverBackground) {
        ui->gameOverBackground->setGeometry(ui->gameOverPage->rect());
    }
    if (ui->winBackgroundLabel) {
        ui->winBackgroundLabel->setGeometry(ui->winPage->rect());
    }
    if (ui->characterBackground) {
        ui->characterBackground->setGeometry(ui->characterSetupScreen->rect());
    }

    if (ui->backgroundFrame) {
        ui->backgroundFrame->setStyleSheet(
            QString("QFrame#backgroundFrame { border-image: url(:/images/background.png) %1; }").arg(stretch));
        ui->backgroundFrame->show();
        ui->backgroundFrame->lower();
    }
    if (ui->menuOverlayFrame)
        ui->menuOverlayFrame->raise();
    if (ui->characterBackground) {
        ui->characterBackground->setPixmap(loadGamePixmap(":/images/characterPage.png"));
        ui->characterBackground->setScaledContents(true);
        ui->characterBackground->show();
        ui->characterBackground->lower();
    }

    // Keep interactive overlays above background layers.
    if (ui->setupOverlayFrame) ui->setupOverlayFrame->raise();
    if (ui->portraitLabel) ui->portraitLabel->raise();
    if (ui->lineEdit) ui->lineEdit->raise();
    if (ui->confirmButton) ui->confirmButton->raise();
    // Ensure character/trait buttons stay clickable.
    if (ui->zaraButton) ui->zaraButton->raise();
    if (ui->karimButton) ui->karimButton->raise();
    if (ui->nourButton) ui->nourButton->raise();
    if (ui->ramiButton) ui->ramiButton->raise();
    if (ui->scholarButton) ui->scholarButton->raise();
    if (ui->survivorButton) ui->survivorButton->raise();
    if (ui->speedsterButton) ui->speedsterButton->raise();
}

MainWindow::~MainWindow()
{
    if (m_game && m_game->player() && m_game->player()->getScore() > 0)
        SaveManager::saveGame(m_game->player(), m_game->currentLevel());
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    applyPageBackgrounds();
    layoutGamePage();
}

void MainWindow::updateHUDDefaults()
{
    ui->hudPortraitLabel->setPixmap(loadGamePixmap(":/images/Zara.png"));
    ui->hudNameLabel->setText("ZARA");
    ui->hudTraitLabel->setText("SCHOLAR");
    ui->livesLabel->setText("♥ ♥ ♥");
    ui->timerLabel->setText("07:32");
    ui->scoreLabel->setText("🏆 0");
    ui->levelLabel->setText("1 / 6");
    ui->roomNameLabel->setText("Temple Entrance");
    ui->eventLabel->setText("A cold wind echoes through the chamber...");
    ui->controlsLabel->setText("Arrow Keys: Move  |  ESC: Pause");
    ui->inventorySlot1->setPixmap(loadGamePixmap(":/images/emptySlot.png"));
    ui->inventorySlot2->setPixmap(loadGamePixmap(":/images/emptySlot.png"));
    ui->inventorySlot3->setPixmap(loadGamePixmap(":/images/emptySlot.png"));
    ui->inventorySlot4->setPixmap(loadGamePixmap(":/images/emptySlot.png"));
}

void MainWindow::setupMenuButtons()
{
    connect(ui->startButton, &QPushButton::clicked, this, [=]() {
        if (m_game) m_game->transitionTo(GameState::CHARACTER_SETUP);
    });

    connect(ui->exitButton, &QPushButton::clicked, this, [=]() { close(); });

    connect(ui->continueButton, &QPushButton::clicked, this, [=]() {
        if (!SaveManager::hasSaveFile()) {
            ui->eventLabel->setText("No saved game found yet.");
            return;
        }

        Player loaded;
        int savedLevel = 1;
        if (!SaveManager::loadGame(&loaded, savedLevel)) {
            ui->eventLabel->setText("Save file exists but could not be loaded.");
            return;
        }

        m_selectedDifficulty = loaded.getDifficulty();
        m_selectedTrait = (loaded.getTrait() == CharacterTrait::SCHOLAR)  ? "Scholar" :
                          (loaded.getTrait() == CharacterTrait::SURVIVOR) ? "Survivor" : "Speedster";
        m_selectedCharacter = (loaded.getCharacterID() == CharacterID::ZARA)  ? "Zara" :
                              (loaded.getCharacterID() == CharacterID::KARIM) ? "Karim" :
                              (loaded.getCharacterID() == CharacterID::NOUR)  ? "Nour" : "Rami";

        m_game->startNewGame(loaded.getDifficulty(),
                             loaded.getTrait(),
                             loaded.getCharacterID(),
                             loaded.getCharacterName(),
                             loaded.getAvatarPath());

        for (int level = 1; level < savedLevel; ++level) {
            if (m_game->roomManager())
                m_game->roomManager()->loadNextRoom();
        }

        setupPlayerHUDSignals();
        if (m_game) m_game->transitionTo(GameState::PLAYING);
        QGraphicsView* gv = findChild<QGraphicsView*>("graphicsView");
        if (gv) {
            gv->setFocus();
            if (Room* room = qobject_cast<Room*>(gv->scene()))
                room->focusPlayer();
        }
        updateInventoryHUD();
    });

    connect(ui->difficultyButton, &QPushButton::clicked, this, [=]() {
        DifficultyDialog dialog(this);
        connect(&dialog, &DifficultyDialog::difficultySelected,
                this, [=](Difficulty d) {
                    m_selectedDifficulty = d;
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

    connect(ui->howToPlayButton, &QPushButton::clicked, this, [=]() {
        HowToPlayDialog dialog(this);
        dialog.exec();
    });

    // enterTombButton: switch page AND give graphics view focus for keyboard input
    connect(ui->enterTombButton, &QPushButton::clicked, this, [=]() {
        if (m_game) m_game->transitionTo(GameState::PLAYING);
        QGraphicsView* gv = findChild<QGraphicsView*>("graphicsView");
        if (gv) {
            gv->setFocus();
            if (Room* room = qobject_cast<Room*>(gv->scene()))
                room->focusPlayer();
        }
    });

    connect(ui->tryAgainButton, &QPushButton::clicked, this, [=]() {
        m_game->restartGame();
        setupPlayerHUDSignals();
        if (m_game) m_game->transitionTo(GameState::PLAYING);
        QGraphicsView* gv = findChild<QGraphicsView*>("graphicsView");
        if (gv) {
            gv->setFocus();
            if (Room* room = qobject_cast<Room*>(gv->scene()))
                room->focusPlayer();
        }
    });

    connect(ui->returnMenuButton, &QPushButton::clicked, this, [=]() {
        if (m_game) m_game->transitionTo(GameState::MENU);
    });

    connect(ui->playAgainButton, &QPushButton::clicked, this, [=]() {
        m_game->restartGame();
        setupPlayerHUDSignals();
        if (m_game) m_game->transitionTo(GameState::PLAYING);
        QGraphicsView* gv = findChild<QGraphicsView*>("graphicsView");
        if (gv) {
            gv->setFocus();
            if (Room* room = qobject_cast<Room*>(gv->scene()))
                room->focusPlayer();
        }
    });

    connect(ui->leaveTempleButton, &QPushButton::clicked, this, [=]() {
        if (m_game) m_game->transitionTo(GameState::MENU);
    });
}

void MainWindow::setupHUDButtons()
{
    connect(ui->pauseTestButton, &QPushButton::clicked, this, [=]() {
        m_game->pauseGame();

        PauseMenuDialog dialog(this);

        connect(&dialog, &PauseMenuDialog::resumeClicked, this, [=]() {
            m_game->resumeGame();
            QGraphicsView* gv = findChild<QGraphicsView*>("graphicsView");
            if (gv) {
                gv->setFocus();
                if (Room* room = qobject_cast<Room*>(gv->scene()))
                    room->focusPlayer();
            }
        });

        connect(&dialog, &PauseMenuDialog::restartClicked, this, [=]() {
            m_game->restartGame();
            setupPlayerHUDSignals();
            if (m_game) m_game->transitionTo(GameState::PLAYING);
        });

        connect(&dialog, &PauseMenuDialog::mainMenuClicked, this, [=]() {
            if (m_game) m_game->transitionTo(GameState::MENU);
        });

        connect(&dialog, &PauseMenuDialog::exitClicked, this, [=]() {
            close();
        });

        dialog.exec();
    });
}

void MainWindow::setupCharacterButtons()
{
    connect(ui->zaraButton, &QPushButton::clicked, this, [=]() {
        m_selectedCharacter = "Zara";
        ui->portraitLabel->setPixmap(loadGamePixmap(":/images/Zara.png"));
        ui->descriptionLabel->setText("Fearless explorer driven by curiosity and courage.");
    });

    connect(ui->karimButton, &QPushButton::clicked, this, [=]() {
        m_selectedCharacter = "Karim";
        ui->portraitLabel->setPixmap(loadGamePixmap(":/images/Karim.png"));
        ui->descriptionLabel->setText("Balanced adventurer skilled in combat and survival.");
    });

    connect(ui->nourButton, &QPushButton::clicked, this, [=]() {
        m_selectedCharacter = "Nour";
        ui->portraitLabel->setPixmap(loadGamePixmap(":/images/NOOR.png"));
        ui->descriptionLabel->setText("Strategic thinker who uncovers hidden ancient secrets.");
    });

    connect(ui->ramiButton, &QPushButton::clicked, this, [=]() {
        m_selectedCharacter = "Rami";
        ui->portraitLabel->setPixmap(loadGamePixmap(":/images/RAMI.png"));
        ui->descriptionLabel->setText("Fast and daring explorer who trusts instinct.");
    });

    connect(ui->scholarButton, &QPushButton::clicked, this, [=]() {
        m_selectedTrait = "Scholar";
        ui->traitDescriptionLabel->setText("Gain intelligence bonuses and uncover hidden clues.");
        ui->traitIconLabel->setPixmap(loadGamePixmap(":/images/scholarIcon.png"));
    });

    connect(ui->survivorButton, &QPushButton::clicked, this, [=]() {
        m_selectedTrait = "Survivor";
        ui->traitDescriptionLabel->setText("Higher endurance and resistance to danger.");
        ui->traitIconLabel->setPixmap(loadGamePixmap(":/images/survivorIcon.png"));
    });

    connect(ui->speedsterButton, &QPushButton::clicked, this, [=]() {
        m_selectedTrait = "Speedster";
        ui->traitDescriptionLabel->setText("Move faster and escape traps more easily.");
        ui->traitIconLabel->setPixmap(loadGamePixmap(":/images/speedsterIcon.png"));
    });
}

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

        ui->hudNameLabel->setText(playerName.toUpper());
        ui->hudTraitLabel->setText(m_selectedTrait.toUpper());
        if (m_selectedCharacter == "Zara")
            ui->hudPortraitLabel->setPixmap(loadGamePixmap(":/images/Zara.png"));
        else if (m_selectedCharacter == "Karim")
            ui->hudPortraitLabel->setPixmap(loadGamePixmap(":/images/Karim.png"));
        else if (m_selectedCharacter == "Nour")
            ui->hudPortraitLabel->setPixmap(loadGamePixmap(":/images/NOOR.png"));
        else
            ui->hudPortraitLabel->setPixmap(loadGamePixmap(":/images/RAMI.png"));

        startGame();
        if (m_game) {
            m_game->pauseGame();
            m_game->transitionTo(GameState::STORY);
        }
    });
}

void MainWindow::startGame()
{
    CharacterID charID =
        (m_selectedCharacter == "Zara")  ? CharacterID::ZARA  :
        (m_selectedCharacter == "Karim") ? CharacterID::KARIM :
        (m_selectedCharacter == "Nour")  ? CharacterID::NOUR  : CharacterID::RAMI;

    CharacterTrait trait =
        (m_selectedTrait == "Scholar")  ? CharacterTrait::SCHOLAR  :
        (m_selectedTrait == "Survivor") ? CharacterTrait::SURVIVOR : CharacterTrait::SPEEDSTER;

    QString avatarPath = !m_pendingAvatarPath.isEmpty()
                             ? m_pendingAvatarPath
                             : (m_game->player() ? m_game->player()->getAvatarPath() : "");

    m_game->startNewGame(m_selectedDifficulty, trait, charID,
                         ui->lineEdit->text(), avatarPath);

    setupPlayerHUDSignals();
}

void MainWindow::setupPlayerHUDSignals()
{
    Player* p = m_game->player();
    if (!p) return;

    connect(p, &Player::timerTick, this, [=](int secondsLeft) {
        int mins = secondsLeft / 60, secs = secondsLeft % 60;
        ui->timerLabel->setText(
            QString("%1:%2").arg(mins).arg(secs, 2, 10, QChar('0')));
        updateInventoryHUD();
    });

    connect(p, &Player::livesChanged, this, [=](int lives) {
        QString hearts = "";
        for (int i = 0; i < lives; i++) hearts += "♥ ";
        ui->livesLabel->setText(hearts.trimmed());
    });

    connect(p, &Player::scoreChanged, this, [=](int score) {
        ui->scoreLabel->setText("🏆 " + QString::number(score));
    });

    if (m_game->roomManager()) {
        connect(m_game->roomManager(), &RoomManager::roomEntered, this,
                [=](int level, const QString& chamberName, RoomMood mood, bool guardianSpawned) {
                    ui->levelLabel->setText(QString("%1 / 6").arg(level));
                    ui->roomNameLabel->setText(chamberName);
                    QString moodText = (mood == RoomMood::CALM) ? "Calm" :
                                       (mood == RoomMood::CURSED) ? "Cursed" : "Blessed";
                    ui->eventLabel->setText(
                        QString("Room mood: %1. %2 Objective: collect artefacts, solve the altar riddle, then unlock the door.")
                            .arg(moodText, guardianSpawned
                                               ? "A guardian presence is sensed."
                                               : "No guardian detected yet."));
                });
    }

    updateInventoryHUD();
}

void MainWindow::onGameEnded(bool won)
{
    if (won) {
        if (m_game) m_game->transitionTo(GameState::WIN);
        if (m_game && m_game->player())
            ui->winScoreLabel->setText(QString::number(m_game->player()->getScore()));
    } else {
        if (m_game) m_game->transitionTo(GameState::GAMEOVER);
        if (m_game && m_game->player())
            ui->label_2->setText(QString::number(m_game->player()->getScore()));
    }
}

void MainWindow::setUiState(GameState state)
{
    if (!ui || !ui->stackedWidget) return;
    switch (state) {
    case GameState::MENU:
        ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);
        break;
    case GameState::CHARACTER_SETUP:
        ui->stackedWidget->setCurrentWidget(ui->characterSetupScreen);
        break;
    case GameState::STORY:
        ui->stackedWidget->setCurrentWidget(ui->storyPage);
        break;
    case GameState::PLAYING:
    case GameState::PAUSED:
        ui->stackedWidget->setCurrentWidget(ui->gamePage);
        break;
    case GameState::GAMEOVER:
        ui->stackedWidget->setCurrentWidget(ui->gameOverPage);
        break;
    case GameState::WIN:
        ui->stackedWidget->setCurrentWidget(ui->winPage);
        break;
    }
}

QStringList MainWindow::validateCriticalAssets() const
{
    const QStringList required = {
        ":/images/background.png", ":/images/characterPage.png", ":/images/storyBackground.png",
        ":/images/gameBackground.png", ":/images/gameover_background.png", ":/images/winBackground.png",
        ":/images/Zara.png", ":/images/Karim.png", ":/images/NOOR.png", ":/images/RAMI.png",
        ":/images/emptySlot.png", ":/images/room1.png", ":/images/room2.png", ":/images/room3.png",
        ":/images/room4.png", ":/images/room5.png", ":/images/room6.png"
    };
    QStringList missing;
    for (const QString& path : required) {
        if (loadGamePixmap(path).isNull()) {
            missing << path;
        }
    }
    return missing;
}

void MainWindow::layoutGamePage()
{
    if (!ui || !ui->gamePage) return;

    const QRect page = ui->gamePage->rect();
    const int margin = 16;
    const int leftPanelW = qBound(220, page.width() / 5, 280);
    const int topBarH = 64;
    const int bottomH = qBound(96, page.height() / 7, 130);

    if (ui->topBarFrame)
        ui->topBarFrame->setGeometry(margin, margin, page.width() - (margin * 2), topBarH);

    if (ui->templeMapTitle)
        ui->templeMapTitle->setGeometry(
            margin,
            margin + topBarH + 8,
            leftPanelW,
            page.height() - topBarH - (margin * 3) - 8);

    const int sceneX = margin + leftPanelW + 14;
    const int sceneY = margin + topBarH + 8;
    const int sceneW = page.width() - sceneX - margin;
    const int sceneH = page.height() - sceneY - bottomH - margin - 10;
    if (ui->graphicsView) {
        ui->graphicsView->setGeometry(sceneX, sceneY, qMax(760, sceneW), qMax(440, sceneH));
        if (ui->graphicsView->scene())
            ui->graphicsView->fitInView(ui->graphicsView->scene()->sceneRect(), Qt::KeepAspectRatio);
    }

    if (ui->bottomStripFrame)
        ui->bottomStripFrame->setGeometry(sceneX, page.height() - bottomH - margin, sceneW, bottomH);

    if (ui->pauseTestButton)
        ui->pauseTestButton->setGeometry(page.width() - 120, margin + topBarH + 14, 100, 34);
}

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
        ui->descriptionLabel->setText("AI generation failed. Your uploaded photo will be used instead.");
    });

    QPushButton* uploadBtn = findChild<QPushButton*>("uploadButton");
    if (!uploadBtn && ui->characterSetupScreen) {
        uploadBtn = new QPushButton("Upload Photo (AI Avatar)", ui->characterSetupScreen);
        uploadBtn->setObjectName("uploadButton");
        uploadBtn->setGeometry(760, 670, 260, 44);
        uploadBtn->setStyleSheet(
            "QPushButton { background-color: rgba(255, 140, 0, 200); color: white; "
            "border-radius: 10px; font-size: 14px; font-weight: bold; } "
            "QPushButton:hover { background-color: rgba(255, 180, 50, 230); }");
        uploadBtn->show();
    }

    if (uploadBtn) {
        connect(uploadBtn, &QPushButton::clicked, this, [=]() {
            QString path = QFileDialog::getOpenFileName(
                this, "Select Photo", "", "Images (*.png *.jpg *.jpeg)");
            if (!path.isEmpty()) {
                m_pendingAvatarPath = path;
                QPixmap local(path);
                if (!local.isNull()) {
                    ui->portraitLabel->setPixmap(
                        local.scaled(ui->portraitLabel->size(),
                                     Qt::KeepAspectRatio, Qt::SmoothTransformation));
                }
                m_avatarGen->generateFromPhoto(path);
            }
        });
    } else {
        QMessageBox::warning(this, "Avatar Setup",
                             "Upload button could not be created. AI avatar generation is unavailable.");
    }
}

void MainWindow::onAvatarReady(QString path)
{
    m_pendingAvatarPath = path;
    if (m_game->player()) m_game->player()->setAvatarPath(path);
    QPixmap avatar(path);
    if (!avatar.isNull()) {
        ui->portraitLabel->setPixmap(
            avatar.scaled(ui->portraitLabel->size(),
                          Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    ui->descriptionLabel->setText("Portrait ready! Confirm to enter the tomb.");
}

QString MainWindow::artefactIconForType(ArtefactType type) const
{
    switch (type) {
    case ArtefactType::EYE_OF_HORUS:     return ":/images/ankh.png";
    case ArtefactType::HOURGLASS:        return ":/images/difficulty.png";
    case ArtefactType::SCARAB_OF_RA:     return ":/images/ankh.png";
    case ArtefactType::SCROLL_OF_ANUBIS: return ":/images/manuscript.png";
    case ArtefactType::BLADE_OF_SEKHMET: return ":/images/manuscript.png";
    default:                             return ":/images/emptySlot.png";
    }
}

void MainWindow::updateInventoryHUD()
{
    QLabel* inventorySlots[4] = { ui->inventorySlot1, ui->inventorySlot2, ui->inventorySlot3, ui->inventorySlot4 };
    for (QLabel* slotLabel : inventorySlots)
        slotLabel->setPixmap(loadGamePixmap(":/images/emptySlot.png"));

    if (!m_game || !m_game->player())
        return;

    const QVector<Artefact*> inv = m_game->player()->getInventory();
    for (int i = 0; i < inv.size() && i < 4; ++i) {
        if (!inv[i]) continue;
        inventorySlots[i]->setPixmap(QPixmap(artefactIconForType(inv[i]->getType())));
    }
}
