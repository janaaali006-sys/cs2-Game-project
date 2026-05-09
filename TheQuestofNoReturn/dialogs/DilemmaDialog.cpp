#include "DilemmaDialog.h"
#include "../entities/Player.h"
#include "../outcomes/Outcome.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

DilemmaDialog::DilemmaDialog(QString scenario,
                             Outcome* optionA,
                             Outcome* optionB,
                             Outcome* optionC,
                             Player* player,
                             QWidget* parent)
    : QDialog(parent),
      m_player(player),
      m_a(optionA),
      m_b(optionB),
      m_c(optionC)
{
    setModal(true);
    setStyleSheet("background-color:#0d0a05; color:#e2cfa0;");

    QVBoxLayout* layout = new QVBoxLayout(this);

    m_text = new QLabel("What will you do?\n\n" + scenario);
    m_text->setAlignment(Qt::AlignCenter);

    m_btnA = new QPushButton("A");
    m_btnB = new QPushButton("B");
    m_btnC = new QPushButton("C");

    m_btnA->setStyleSheet("background-color:#c8a03c;");
    m_btnB->setStyleSheet("background-color:#8b0000; color:white;");
    m_btnC->setStyleSheet("background-color:#c8a03c;");

    layout->addWidget(m_text);
    layout->addWidget(m_btnA);
    layout->addWidget(m_btnB);
    layout->addWidget(m_btnC);

    connect(m_btnA, &QPushButton::clicked, this, &DilemmaDialog::chooseA);
    connect(m_btnB, &QPushButton::clicked, this, &DilemmaDialog::chooseB);
    connect(m_btnC, &QPushButton::clicked, this, &DilemmaDialog::chooseC);
}

void DilemmaDialog::applyOutcome(Outcome* outcome) {
    outcome->apply(m_player);
    accept();
}

void DilemmaDialog::chooseA() { applyOutcome(m_a); }
void DilemmaDialog::chooseB() { applyOutcome(m_b); }
void DilemmaDialog::chooseC() { applyOutcome(m_c); }
