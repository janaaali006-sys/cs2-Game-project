#include "howtoplaydialog.h"
#include "ui_howtoplaydialog.h"

HowToPlayDialog::HowToPlayDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HowToPlayDialog)
{
    ui->setupUi(this);

    connect(ui->closeButton, &QPushButton::clicked,
            this, [=]() {

                close();

            });
}

HowToPlayDialog::~HowToPlayDialog()
{
    delete ui;
}
