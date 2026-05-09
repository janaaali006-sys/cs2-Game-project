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
    // PORTRAIT SETTINGS
    // =====================================================

    ui->portraitLabel->setScaledContents(true);

    ui->traitIconLabel->setScaledContents(true);



    // =====================================================
    // DEFAULT CHARACTER
    // =====================================================

    selectedCharacter = "Zara";

    ui->portraitLabel->setPixmap(
        QPixmap(":/images/Zara.png")
        );



    // =====================================================
    // DEFAULT TRAIT ICON
    // =====================================================

    ui->traitIconLabel->setPixmap(
        QPixmap(":/images/scholarIcon.png")
        );



    // =====================================================
    // DEFAULT TEXT
    // =====================================================

    ui->descriptionLabel->setText(
        "Fearless explorer driven by curiosity and courage."
        );

    ui->traitDescriptionLabel->setText(
        "Gain intelligence bonuses and uncover hidden clues."
        );



    // =====================================================
    // CHARACTER BUTTONS
    // =====================================================

    // ZARA
    connect(ui->zaraButton, &QPushButton::clicked, this, [=]() {

        selectedCharacter = "Zara";

        ui->portraitLabel->setPixmap(
            QPixmap(":/images/Zara.png")
            );

        ui->descriptionLabel->setText(
            "Fearless explorer driven by curiosity and courage."
            );
    });



    // KARIM
    connect(ui->karimButton, &QPushButton::clicked, this, [=]() {

        selectedCharacter = "Karim";

        ui->portraitLabel->setPixmap(
            QPixmap(":/images/Karim.png")
            );

        ui->descriptionLabel->setText(
            "Balanced adventurer skilled in combat and survival."
            );
    });



    // NOUR
    connect(ui->nourButton, &QPushButton::clicked, this, [=]() {

        selectedCharacter = "Nour";

        ui->portraitLabel->setPixmap(
            QPixmap(":/images/NOOR.png")
            );

        ui->descriptionLabel->setText(
            "Strategic thinker who uncovers hidden ancient secrets."
            );
    });



    // RAMI
    connect(ui->ramiButton, &QPushButton::clicked, this, [=]() {

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
    connect(ui->scholarButton, &QPushButton::clicked, this, [=]() {

        selectedTrait = "Scholar";

        ui->traitDescriptionLabel->setText(
            "Gain intelligence bonuses and uncover hidden clues."
            );

        ui->traitIconLabel->setPixmap(
            QPixmap(":/images/scholarIcon.png")
            );
    });



    // SURVIVOR
    connect(ui->survivorButton, &QPushButton::clicked, this, [=]() {

        selectedTrait = "Survivor";

        ui->traitDescriptionLabel->setText(
            "Higher endurance and resistance to danger."
            );

        ui->traitIconLabel->setPixmap(
            QPixmap(":/images/survivorIcon.png")
            );
    });



    // SPEEDSTER
    connect(ui->speedsterButton, &QPushButton::clicked, this, [=]() {

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

    connect(ui->confirmButton, &QPushButton::clicked, this, [=]() {

        QString playerName = ui->lineEdit->text();



        // EMPTY NAME
        if(playerName.isEmpty()) {

            ui->traitDescriptionLabel->setText(
                "Enter your explorer name first."
                );

            return;
        }



        // NO CHARACTER
        if(selectedCharacter.isEmpty()) {

            ui->traitDescriptionLabel->setText(
                "Choose your explorer first."
                );

            return;
        }



        // NO TRAIT
        if(selectedTrait.isEmpty()) {

            ui->traitDescriptionLabel->setText(
                "Choose a trait before entering the tomb."
                );

            return;
        }



        // FINAL CONFIRMATION
        ui->traitDescriptionLabel->setText(

            "Explorer " + playerName +
            " the " + selectedCharacter +
            " enters the tomb as a " +
            selectedTrait + "."

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



// =====================================================
// START BUTTON
// =====================================================

void MainWindow::on_startButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(
        ui->characterSetupScreen
        );
}
