#include "mainwindow.h"
#include "ui_mainwindow.h"

QString selectedCharacter = "";
QString selectedTrait = "";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    // =====================================================
    // START ON MAIN MENU
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
    // INVENTORY SLOT SETTINGS
    // =====================================================

    ui->inventorySlot1->setScaledContents(true);
    ui->inventorySlot2->setScaledContents(true);
    ui->inventorySlot3->setScaledContents(true);
    ui->inventorySlot4->setScaledContents(true);

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
    // START GAME BUTTON
    // =====================================================

    connect(ui->startButton, &QPushButton::clicked,
            this, [=]() {

                ui->stackedWidget->setCurrentWidget(
                    ui->characterSetupScreen
                    );

            });



    // =====================================================
    // EXIT BUTTON
    // =====================================================

    connect(ui->exitButton, &QPushButton::clicked,
            this, [=]() {

                close();

            });



    // =====================================================
    // CHARACTER BUTTONS
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
    // TRAIT BUTTONS
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
    // CONFIRM BUTTON
    // =====================================================

    connect(ui->confirmButton, &QPushButton::clicked,
            this, [=]() {

                QString playerName = ui->lineEdit->text();



                // EMPTY NAME
                if(playerName.isEmpty()) {

                    ui->traitDescriptionLabel->setText(
                        "Enter your explorer name first."
                        );

                    return;
                }



                // =================================================
                // UPDATE HUD NAME + TRAIT
                // =================================================

                ui->hudNameLabel->setText(
                    playerName.toUpper()
                    );

                ui->hudTraitLabel->setText(
                    selectedTrait.toUpper()
                    );



                // =================================================
                // UPDATE HUD PORTRAIT
                // =================================================

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



                // =================================================
                // GO TO STORY PAGE
                // =================================================

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

}



// =====================================================
// DESTRUCTOR
// =====================================================

MainWindow::~MainWindow()
{
    delete ui;
}
