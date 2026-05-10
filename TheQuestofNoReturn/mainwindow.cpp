#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pausemenudialog.h"
#include "difficultydialog.h"
#include "howtoplaydialog.h"

QString selectedCharacter = "";
QString selectedTrait = "";



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    // =====================================================
    // START PAGE
    // =====================================================

    ui->stackedWidget->setCurrentWidget(
        ui->mainMenuPage
        );



    // =====================================================
    // IMAGE SETTINGS
    // =====================================================

    ui->portraitLabel->setScaledContents(true);
    ui->traitIconLabel->setScaledContents(true);
    ui->hudPortraitLabel->setScaledContents(true);

    ui->inventorySlot1->setScaledContents(true);
    ui->inventorySlot2->setScaledContents(true);
    ui->inventorySlot3->setScaledContents(true);
    ui->inventorySlot4->setScaledContents(true);



    // =====================================================
    // DEFAULT PLAYER DATA
    // =====================================================

    selectedCharacter = "Zara";
    selectedTrait = "Scholar";



    // =====================================================
    // CHARACTER SETUP DEFAULTS
    // =====================================================

    ui->portraitLabel->setPixmap(
        QPixmap(":/images/Zara.png")
        );

    ui->traitIconLabel->setPixmap(
        QPixmap(":/images/scholarIcon.png")
        );

    ui->descriptionLabel->setText(
        "Fearless explorer driven by curiosity and courage."
        );

    ui->traitDescriptionLabel->setText(
        "Gain intelligence bonuses and uncover hidden clues."
        );



    // =====================================================
    // GAME HUD DEFAULTS
    // =====================================================

    ui->hudPortraitLabel->setPixmap(
        QPixmap(":/images/Zara.png")
        );

    ui->hudNameLabel->setText(
        "ZARA"
        );

    ui->hudTraitLabel->setText(
        "SCHOLAR"
        );

    ui->livesLabel->setText(
        "♥ ♥ ♥"
        );

    ui->timerLabel->setText(
        "07:32"
        );

    ui->scoreLabel->setText(
        "🏆 0"
        );

    ui->levelLabel->setText(
        "1 / 6"
        );

    ui->roomNameLabel->setText(
        "Temple Entrance"
        );

    ui->eventLabel->setText(
        "A cold wind echoes through the chamber..."
        );

    ui->controlsLabel->setText(
        "WASD Move | E Interact | I Inventory | ESC Pause"
        );



    // =====================================================
    // INVENTORY DEFAULT ICONS
    // =====================================================

    ui->inventorySlot1->setPixmap(
        QPixmap(":/images/emptySlot.png")
        );

    ui->inventorySlot2->setPixmap(
        QPixmap(":/images/emptySlot.png")
        );

    ui->inventorySlot3->setPixmap(
        QPixmap(":/images/emptySlot.png")
        );

    ui->inventorySlot4->setPixmap(
        QPixmap(":/images/emptySlot.png")
        );



    // =====================================================
    // MAIN MENU BUTTONS
    // =====================================================

    // START GAME
    connect(ui->startButton, &QPushButton::clicked,
            this, [=]() {

                ui->stackedWidget->setCurrentWidget(
                    ui->characterSetupScreen
                    );

            });



    // EXIT GAME
    connect(ui->exitButton, &QPushButton::clicked,
            this, [=]() {

                close();

            });



    // =====================================================
    // CHARACTER SELECTION
    // =====================================================

    // ZARA
    connect(ui->zaraButton, &QPushButton::clicked,
            this, [=]() {

                selectedCharacter = "Zara";

                ui->portraitLabel->setPixmap(
                    QPixmap(":/images/Zara.png")
                    );

                ui->descriptionLabel->setText(
                    "Fearless explorer driven by curiosity and courage."
                    );

            });



    // KARIM
    connect(ui->karimButton, &QPushButton::clicked,
            this, [=]() {

                selectedCharacter = "Karim";

                ui->portraitLabel->setPixmap(
                    QPixmap(":/images/Karim.png")
                    );

                ui->descriptionLabel->setText(
                    "Balanced adventurer skilled in combat and survival."
                    );

            });



    // NOUR
    connect(ui->nourButton, &QPushButton::clicked,
            this, [=]() {

                selectedCharacter = "Nour";

                ui->portraitLabel->setPixmap(
                    QPixmap(":/images/NOOR.png")
                    );

                ui->descriptionLabel->setText(
                    "Strategic thinker who uncovers hidden ancient secrets."
                    );

            });



    // RAMI
    connect(ui->ramiButton, &QPushButton::clicked,
            this, [=]() {

                selectedCharacter = "Rami";

                ui->portraitLabel->setPixmap(
                    QPixmap(":/images/RAMI.png")
                    );

                ui->descriptionLabel->setText(
                    "Fast and daring explorer who trusts instinct."
                    );

            });



    // =====================================================
    // TRAIT SELECTION
    // =====================================================

    // SCHOLAR
    connect(ui->scholarButton, &QPushButton::clicked,
            this, [=]() {

                selectedTrait = "Scholar";

                ui->traitDescriptionLabel->setText(
                    "Gain intelligence bonuses and uncover hidden clues."
                    );

                ui->traitIconLabel->setPixmap(
                    QPixmap(":/images/scholarIcon.png")
                    );

            });



    // SURVIVOR
    connect(ui->survivorButton, &QPushButton::clicked,
            this, [=]() {

                selectedTrait = "Survivor";

                ui->traitDescriptionLabel->setText(
                    "Higher endurance and resistance to danger."
                    );

                ui->traitIconLabel->setPixmap(
                    QPixmap(":/images/survivorIcon.png")
                    );

            });



    // SPEEDSTER
    connect(ui->speedsterButton, &QPushButton::clicked,
            this, [=]() {

                selectedTrait = "Speedster";

                ui->traitDescriptionLabel->setText(
                    "Move faster and escape traps more easily."
                    );

                ui->traitIconLabel->setPixmap(
                    QPixmap(":/images/speedsterIcon.png")
                    );

            });



    // =====================================================
    // CONFIRM CHARACTER BUTTON
    // =====================================================

    connect(ui->confirmButton, &QPushButton::clicked,
            this, [=]() {

                QString playerName = ui->lineEdit->text();



                if(playerName.isEmpty()) {

                    ui->traitDescriptionLabel->setText(
                        "Enter your explorer name first."
                        );

                    return;
                }



                // UPDATE HUD
                ui->hudNameLabel->setText(
                    playerName.toUpper()
                    );

                ui->hudTraitLabel->setText(
                    selectedTrait.toUpper()
                    );



                // UPDATE PORTRAIT
                if(selectedCharacter == "Zara") {

                    ui->hudPortraitLabel->setPixmap(
                        QPixmap(":/images/Zara.png")
                        );
                }

                else if(selectedCharacter == "Karim") {

                    ui->hudPortraitLabel->setPixmap(
                        QPixmap(":/images/Karim.png")
                        );
                }

                else if(selectedCharacter == "Nour") {

                    ui->hudPortraitLabel->setPixmap(
                        QPixmap(":/images/NOOR.png")
                        );
                }

                else if(selectedCharacter == "Rami") {

                    ui->hudPortraitLabel->setPixmap(
                        QPixmap(":/images/RAMI.png")
                        );
                }



                // GO TO STORY PAGE
                ui->stackedWidget->setCurrentWidget(
                    ui->storyPage
                    );

            });



    // =====================================================
    // ENTER TOMB BUTTON
    // =====================================================

    connect(ui->enterTombButton, &QPushButton::clicked,
            this, [=]() {

                ui->stackedWidget->setCurrentWidget(
                    ui->gamePage
                    );

            });



    // =====================================================
    // PAUSE MENU BUTTON
    // =====================================================

    connect(ui->pauseTestButton, &QPushButton::clicked,
            this, [=]() {

                PauseMenuDialog dialog(this);



                // RESUME
                connect(&dialog, &PauseMenuDialog::resumeClicked,
                        this, [=]() {

                            ui->stackedWidget->setCurrentWidget(
                                ui->gamePage
                                );

                        });



                // RESTART
                connect(&dialog, &PauseMenuDialog::restartClicked,
                        this, [=]() {

                            ui->stackedWidget->setCurrentWidget(
                                ui->gamePage
                                );

                        });



                // MAIN MENU
                connect(&dialog, &PauseMenuDialog::mainMenuClicked,
                        this, [=]() {

                            ui->stackedWidget->setCurrentWidget(
                                ui->mainMenuPage
                                );

                        });



                // EXIT
                connect(&dialog, &PauseMenuDialog::exitClicked,
                        this, [=]() {

                            close();

                        });



                dialog.exec();

            });



    // =====================================================
    // GAME OVER BUTTONS
    // =====================================================

    // TRY AGAIN
    connect(ui->tryAgainButton, &QPushButton::clicked,
            this, [=]() {

                ui->stackedWidget->setCurrentWidget(
                    ui->gamePage
                    );

            });



    // RETURN TO MAIN MENU
    connect(ui->returnMenuButton, &QPushButton::clicked,
            this, [=]() {

                ui->stackedWidget->setCurrentWidget(
                    ui->mainMenuPage
                    );

            });



    // =====================================================
    // WIN SCREEN BUTTONS
    // =====================================================

    // PLAY AGAIN
    connect(ui->playAgainButton, &QPushButton::clicked,
            this, [=]() {

                ui->stackedWidget->setCurrentWidget(
                    ui->gamePage
                    );

            });



    // LEAVE TEMPLE
    connect(ui->leaveTempleButton, &QPushButton::clicked,
            this, [=]() {

                ui->stackedWidget->setCurrentWidget(
                    ui->mainMenuPage
                    );

            });




    // =====================================================
    // DIFFICULTY BUTTON
    // =====================================================

    connect(ui->difficultyButton, &QPushButton::clicked,
            this, [=]() {

                DifficultyDialog dialog(this);



                // =============================================
                // RECEIVE SELECTED DIFFICULTY
                // =============================================

                connect(&dialog,
                        &DifficultyDialog::difficultySelected,
                        this,
                        [=](Difficulty difficulty) {

                            if(difficulty == Difficulty::EASY) {

                                ui->timerLabel->setText("12:00");
                                ui->livesLabel->setText("♥ ♥ ♥ ♥");
                            }

                            else if(difficulty == Difficulty::MEDIUM) {

                                ui->timerLabel->setText("08:00");
                                ui->livesLabel->setText("♥ ♥ ♥");
                            }

                            else if(difficulty == Difficulty::HARD) {

                                ui->timerLabel->setText("05:00");
                                ui->livesLabel->setText("♥ ♥");
                            }

                        });



                dialog.exec();

            });


    // HOW TO PLAY BUTTON

    connect(ui->howToPlayButton, &QPushButton::clicked,
            this, [=]() {

                HowToPlayDialog dialog(this);
                dialog.exec();

            });












}



// =====================================================
// DESTRUCTOR
// =====================================================

MainWindow::~MainWindow()
{
    delete ui;
}
