#include "RevelationDialog.h"
#include "../outcomes/Outcome.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

RevelationDialog::RevelationDialog(Outcome* outcome, QString artefactName, QWidget* parent)
    : QDialog(parent)
{
    setModal(true);

    setStyleSheet("background-color:#0d0a05; color:#e2cfa0;");

    QVBoxLayout* layout = new QVBoxLayout(this);

    m_banner = new QLabel(outcome->isPositive() ? "GOOD CHOICE" : "WRONG CHOICE");
    m_banner->setAlignment(Qt::AlignCenter);

    if (outcome->isPositive()) {
        m_banner->setStyleSheet("color:green; font-size:20px;");
    } else {
        m_banner->setStyleSheet("color:red; font-size:20px;");
    }

    m_description = new QLabel(outcome->getDescription());
    m_description->setAlignment(Qt::AlignCenter);

    m_artefact = new QLabel("");
    if (!artefactName.isEmpty()) {
        m_artefact->setText("You found: " + artefactName);
        m_artefact->setAlignment(Qt::AlignCenter);
        m_artefact->setStyleSheet("color:#c8a03c;");
    }

    m_continueBtn = new QPushButton("Continue");

    layout->addWidget(m_banner);
    layout->addWidget(m_description);
    layout->addWidget(m_artefact);
    layout->addWidget(m_continueBtn);

    connect(m_continueBtn, &QPushButton::clicked, this, &RevelationDialog::accept);
}
