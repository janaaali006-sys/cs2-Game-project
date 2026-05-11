#include "RiddleDialog.h"

#include "../riddles/Riddle.h"
#include "../player.h"
#include "../world/RiddleManager.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QMessageBox>
#include <QCloseEvent>
#include <QHBoxLayout>

RiddleDialog::RiddleDialog(Riddle* riddle, Player* player, QWidget* parent)
    : QDialog(parent),
      m_riddle(riddle),
      m_player(player)
{
    setModal(true);
    setWindowTitle(tr("Temple Riddle"));
    setAttribute(Qt::WA_DeleteOnClose, false);
    setupUI();

    connect(&m_timer, &QTimer::timeout, this, &RiddleDialog::updateTimer);

    if (!m_player) {
        m_timeLeft = 30;
    } else if (m_player->getDifficulty() == Difficulty::EASY) {
        m_timeLeft = 45;
    } else if (m_player->getDifficulty() == Difficulty::MEDIUM) {
        m_timeLeft = 30;
    } else {
        m_timeLeft = 20;
    }

    m_timerBar->setRange(0, m_timeLeft);
    m_timerBar->setValue(m_timeLeft);
    m_timer.start(1000);
}

void RiddleDialog::setManager(RiddleManager* manager)
{
    m_manager = manager;
}

void RiddleDialog::setupUI()
{
    setStyleSheet("background-color:#0d0a05; color:#e2cfa0;");

    QVBoxLayout* layout = new QVBoxLayout(this);

    m_promptLabel = new QLabel(m_riddle ? m_riddle->getPrompt() : tr("No riddle."));
    m_promptLabel->setAlignment(Qt::AlignCenter);
    m_promptLabel->setWordWrap(true);

    m_hintLabel = new QLabel("");
    m_hintLabel->setAlignment(Qt::AlignCenter);
    m_hintLabel->setWordWrap(true);

    m_input = new QLineEdit();
    m_input->setPlaceholderText(tr("Your answer"));

    m_submit = new QPushButton(tr("Submit"));
    m_hintBtn = new QPushButton(tr("Hint"));
    m_timerBar = new QProgressBar();
    m_timerBar->setTextVisible(true);
    m_timerBar->setFormat(tr("%v s left"));

    auto* closeBtn = new QPushButton(tr("Leave (forfeit)"));
    closeBtn->setToolTip(tr("Close without solving — counts as failure."));

    QHBoxLayout* row = new QHBoxLayout();
    row->addWidget(m_submit);
    row->addWidget(m_hintBtn);
    row->addWidget(closeBtn);

    layout->addWidget(m_promptLabel);
    layout->addWidget(m_hintLabel);
    layout->addWidget(m_input);
    layout->addLayout(row);
    layout->addWidget(m_timerBar);

    connect(m_submit, &QPushButton::clicked, this, &RiddleDialog::onSubmit);
    connect(m_hintBtn, &QPushButton::clicked, this, &RiddleDialog::showHint);
    connect(closeBtn, &QPushButton::clicked, this, [this]() {
        if (m_finished)
            return;
        // Forfeit: same rules as failure, with clear feedback (parent nullptr so dialog can close).
        QMessageBox::information(nullptr, tr("Riddle"),
                                 tr("You left the trial. The chamber counts this as failure."));
        fail();
    });

    if (!m_player || !m_player->hintAvailable()) {
        m_hintBtn->setEnabled(false);
        m_hintLabel->setText(tr("No hint available for this room."));
    }
}

void RiddleDialog::closeEvent(QCloseEvent* event)
{
    if (m_finished) {
        QDialog::closeEvent(event);
        return;
    }
    // Title-bar / system close: forfeit (same rules as failure).
    fail();
    event->accept();
}

void RiddleDialog::reject()
{
    if (m_finished) {
        QDialog::reject();
        return;
    }
    // Escape key and other paths that call reject() — count as forfeit failure once.
    fail();
}

void RiddleDialog::onSubmit()
{
    if (m_finished || !m_riddle)
        return;

    if (m_riddle->checkAnswer(m_input->text())) {
        m_timer.stop();
        m_finished = true;

        if (m_manager)
            m_manager->onRiddleSuccess(m_riddle);

        if (m_player)
            m_player->addScore(100);

        QDialog::accept();
        return;
    }

    // Wrong answer: show message with no parent on this dialog (avoids modal deadlock on macOS).
    m_timer.stop();
    QMessageBox::warning(nullptr, tr("Wrong"), tr("Wrong answer!"));
    fail();
}

void RiddleDialog::updateTimer()
{
    if (m_finished)
        return;

    m_timeLeft--;
    m_timerBar->setValue(qMax(0, m_timeLeft));

    if (m_timeLeft <= 0) {
        m_timer.stop();
        QMessageBox::warning(nullptr, tr("Time is up"), tr("The sands claim your answer."));
        fail();
    }
}

void RiddleDialog::showHint()
{
    if (!m_player)
        return;

    if (m_player->hintAvailable() && m_riddle) {
        m_hintLabel->setText(m_riddle->getHint());
        m_player->setHintAvailable(false);
        m_hintBtn->setEnabled(false);
    } else {
        m_hintLabel->setText(tr("Hint already used in this room."));
        m_hintBtn->setEnabled(false);
    }
}

void RiddleDialog::fail()
{
    if (m_finished)
        return;

    m_finished = true;
    m_timer.stop();
    m_timer.disconnect(this);

    if (m_manager)
        m_manager->onRiddleFail();
    if (m_player)
        m_player->loseLife();

    QDialog::reject();
}
