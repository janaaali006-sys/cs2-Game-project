#include "pausemenudialog.h"
#include "ui_pausemenudialog.h"

#include <QApplication>



PauseMenuDialog::PauseMenuDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PauseMenuDialog)
{
    ui->setupUi(this);



    // =====================================================
    // REMOVE WINDOW FRAME
    // =====================================================

    setWindowFlags(
        Qt::FramelessWindowHint
        );



    // =====================================================
    // RESUME BUTTON
    // =====================================================

    connect(ui->resumeButton, &QPushButton::clicked,
            this, [=]() {

                emit resumeClicked();

                close();

            });



    // =====================================================
    // RESTART BUTTON
    // =====================================================

    connect(ui->restartButton, &QPushButton::clicked,
            this, [=]() {

                emit restartClicked();

                close();

            });



    // =====================================================
    // MAIN MENU BUTTON
    // =====================================================

    connect(ui->menuButton, &QPushButton::clicked,
            this, [=]() {

                emit mainMenuClicked();

                close();

            });



    // =====================================================
    // EXIT BUTTON
    // =====================================================

    connect(ui->exitButton, &QPushButton::clicked,
            this, [=]() {

                emit exitClicked();

                QApplication::quit();

            });

}



// =====================================================
// DESTRUCTOR
// =====================================================

PauseMenuDialog::~PauseMenuDialog()
{
    delete ui;
}
