#include "riddledialog.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>

RiddleDialog::RiddleDialog(const Riddle &riddle,
                           const QString &trait,
                           QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setModal(true);
    setFixedSize(560, 400);

    setStyleSheet(
        "QDialog {"
        "  background-color: #0d0905;"
        "  border: 3px solid #c8a03c;"
        "  border-radius: 16px;"
        "}"
    );

    auto *root = new QVBoxLayout(this);
    root->setSpacing(10);
    root->setContentsMargins(24, 20, 24, 20);

    // Title
    auto *title = new QLabel("⚱  Ancient Riddle  ⚱", this);
    title->setStyleSheet(
        "color:#c8a03c; font-size:22px; font-weight:bold;"
        "font-family:Copperplate; background:transparent;");
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    // Question
    auto *q = new QLabel(riddle.question, this);
    q->setStyleSheet(
        "color:#f4e8a0; font-size:14px; font-family:Copperplate; font-weight:bold;"
        "background:rgba(255,200,50,15); border:1px solid rgba(200,160,60,80);"
        "border-radius:10px; padding:12px;");
    q->setWordWrap(true);
    q->setAlignment(Qt::AlignCenter);
    root->addWidget(q);

    // Scholar hint
    if (trait == "Scholar" && !riddle.scholarHint.isEmpty()) {
        auto *hint = new QLabel(riddle.scholarHint, this);
        hint->setStyleSheet(
            "color:#80c8ff; font-size:12px; font-style:italic;"
            "background:rgba(0,60,120,100); border:1px solid rgba(80,140,220,120);"
            "border-radius:8px; padding:6px;");
        hint->setWordWrap(true);
        root->addWidget(hint);
    }

    // 2x2 answer grid
    static const QString BTN =
        "QPushButton{background:rgba(30,20,8,230);color:#e8d080;"
        "border:2px solid #c8a03c;border-radius:10px;"
        "font-size:13px;font-weight:bold;padding:10px;min-height:40px;}"
        "QPushButton:hover{background:rgba(90,65,20,240);border:2px solid #ffe090;}";

    auto *grid = new QGridLayout();
    grid->setSpacing(8);
    for (int i = 0; i < 4; i++) {
        auto *btn = new QPushButton(riddle.choices[i], this);
        btn->setStyleSheet(BTN);
        int correct = riddle.correctIndex;
        connect(btn, &QPushButton::clicked, this, [=]() {
            m_correct = (i == correct);
            accept();
        });
        grid->addWidget(btn, i / 2, i % 2);
    }
    root->addLayout(grid);
}
