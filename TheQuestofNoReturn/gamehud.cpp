#include "gamehud.h"
#include "ui_mainwindow.h"

// Sidebar room label style sheets — identical strings to original mainwindow.cpp
static const QString S_NORMAL =
    "QLabel{background:rgba(15,10,5,180);color:#e2cfa0;"
    "border-left:3px solid #c8a03c;border-radius:6px;"
    "padding-left:8px;font-size:12px;font-family:\"Copperplate\";}";

static const QString S_ACTIVE =
    "QLabel{background:rgba(200,160,60,210);color:#0d0a05;"
    "border-left:4px solid #ffffc0;border-radius:6px;"
    "padding-left:8px;font-size:12px;font-family:\"Copperplate\";font-weight:bold;}";

static const QString S_DONE =
    "QLabel{background:rgba(0,80,20,160);color:#90ffb0;"
    "border-left:3px solid #00cc44;border-radius:6px;"
    "padding-left:8px;font-size:12px;font-family:\"Copperplate\";}";

GameHUD::GameHUD(Ui::MainWindow *ui, QLabel **mapLabels)
    : m_ui(ui), m_mapLabels(mapLabels)
{}

void GameHUD::refreshTimer(int timeSeconds)
{
    int m = timeSeconds / 60, s = timeSeconds % 60;
    m_ui->timerLabel->setText(QString("%1:%2")
        .arg(m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0')));
}

void GameHUD::refreshLives(int lives)
{
    QString h;
    for (int i = 0; i < lives; i++) h += "♥ ";
    m_ui->livesLabel->setText(h.isEmpty() ? "—" : h.trimmed());
}

void GameHUD::refreshMap(int currentRoom)
{
    // 7 rooms, 6 sidebar slots (rooms 0–5); room 6 has no slot
    for (int i = 0; i < 6; i++) {
        bool allDone = (currentRoom > 5);
        if      (allDone || i < currentRoom)  m_mapLabels[i]->setStyleSheet(S_DONE);
        else if (i == currentRoom)            m_mapLabels[i]->setStyleSheet(S_ACTIVE);
        else                                  m_mapLabels[i]->setStyleSheet(S_NORMAL);
    }
}

void GameHUD::setEventText(const QString &msg)
{
    m_ui->eventLabel->setText(msg);
}

void GameHUD::setScore(int score)
{
    m_ui->scoreLabel->setText(QString("🏆 %1").arg(score));
}
