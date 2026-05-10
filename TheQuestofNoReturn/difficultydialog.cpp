#include "difficultydialog.h"
#include "ui_difficultydialog.h"

DifficultyDialog::DifficultyDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DifficultyDialog)
{
    ui->setupUi(this);



    // =====================================================
    // HIDE CONFIRM BUTTON AT START
    // =====================================================

    ui->confirmDifficultyButton->hide();



    // =====================================================
    // EASY CARD
    // =====================================================

    connect(ui->easyCard, &QPushButton::clicked,
            this, [=]() {

                selectedDifficulty = Difficulty::EASY;

                ui->confirmDifficultyButton->show();



                // HIGHLIGHT EASY
                ui->easyCard->setStyleSheet(
                    "border: 6px solid #00FF66;"
                    );

                ui->mediumCard->setStyleSheet("");
                ui->hardCard->setStyleSheet("");

            });



    // =====================================================
    // MEDIUM CARD
    // =====================================================

    connect(ui->mediumCard, &QPushButton::clicked,
            this, [=]() {

                selectedDifficulty = Difficulty::MEDIUM;

                ui->confirmDifficultyButton->show();



                // HIGHLIGHT MEDIUM
                ui->mediumCard->setStyleSheet(
                    "border: 6px solid #FFD700;"
                    );

                ui->easyCard->setStyleSheet("");
                ui->hardCard->setStyleSheet("");

            });



    // =====================================================
    // HARD CARD
    // =====================================================

    connect(ui->hardCard, &QPushButton::clicked,
            this, [=]() {

                selectedDifficulty = Difficulty::HARD;

                ui->confirmDifficultyButton->show();



                // HIGHLIGHT HARD
                ui->hardCard->setStyleSheet(
                    "border: 6px solid #FF2222;"
                    );

                ui->easyCard->setStyleSheet("");
                ui->mediumCard->setStyleSheet("");

            });



    // =====================================================
    // CONFIRM BUTTON
    // =====================================================

    connect(ui->confirmDifficultyButton,
            &QPushButton::clicked,
            this,
            [=]() {

                emit difficultySelected(selectedDifficulty);

                accept();

            });
}

DifficultyDialog::~DifficultyDialog()
{
    delete ui;
}
