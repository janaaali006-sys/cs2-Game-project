#include "RiddleDialog.h"

#include "../riddles/Riddle.h"
#include "../entities/Player.h"
#include "../world/RiddleManager.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QMessageBox>

RiddleDialog::RiddleDialog(Riddle* riddle, Player* player, QWidget* parent)
    : QDialog(parent),
      m_riddle(riddle),
      m_player(player)
{
    setModal(true);
    setupUI();

    connect(&m_timer, &QTimer::timeout, this, &RiddleDialog::updateTimer);

    if (player->getDifficulty() == Difficulty::EASY)
        m_timeLeft = 45;
    else if (player->getDifficulty() == Difficulty::MEDIUM)
        m_timeLeft = 30;
    else
        m_timeLeft = 20;

    m_timer.start(1000);
}

void RiddleDialog::setManager(RiddleManager* manager) {
    m_manager = manager;
}

void RiddleDialog::setupUI() {

    setStyleSheet("background-color:#0d0a05; color:#e2cfa0;");

    QVBoxLayout* layout = new QVBoxLayout(this);

    m_promptLabel = new QLabel(m_riddle->getPrompt());
    m_promptLabel->setAlignment(Qt::AlignCenter);

    m_hintLabel = new QLabel("");
    m_hintLabel->setAlignment(Qt::AlignCenter);

    m_input = new QLineEdit();

    m_submit = new QPushButton("Submit");
    m_hintBtn = new QPushButton("Hint");
    m_timerBar = new QProgressBar();

    layout->addWidget(m_promptLabel);
    layout->addWidget(m_hintLabel);
    layout->addWidget(m_input);
    layout->addWidget(m_submit);
    layout->addWidget(m_hintBtn);
    layout->addWidget(m_timerBar);

    connect(m_submit, &QPushButton::clicked, this, &RiddleDialog::onSubmit);
    connect(m_hintBtn, &QPushButton::clicked, this, &RiddleDialog::showHint);
}

void RiddleDialog::onSubmit() {

    if (m_riddle->checkAnswer(m_input->text())) {

        m_timer.stop();

        if (m_manager)
            m_manager->onRiddleSuccess(m_riddle);

        accept();

    } else {

        fail();
        QMessageBox::warning(this, "Wrong", "Wrong answer!");
    }
}

void RiddleDialog::updateTimer() {

    m_timeLeft--;
    m_timerBar->setValue(m_timeLeft);

    if (m_timeLeft <= 0) {
        fail();
    }
}

void RiddleDialog::showHint() {

    if (m_player->hintAvailable()) {
        m_hintLabel->setText(m_riddle->getHint());
    }
}

void RiddleDialog::fail() {

    m_timer.stop();

    if (m_manager)
        m_manager->onRiddleFail();

    reject();
}
