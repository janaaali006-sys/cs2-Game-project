#ifndef GAMEHUD_H
#define GAMEHUD_H
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class GameHUD : public QWidget {
    Q_OBJECT
public:
    explicit GameHUD(QWidget* parent = nullptr) : QWidget(parent) {
        auto* lay = new QVBoxLayout(this);
        m_livesLabel = new QLabel("Lives: 2", this);
        m_scoreLabel = new QLabel("Score: 0", this);
        m_timerLabel = new QLabel("Time: 8:00", this);
        lay->addWidget(m_livesLabel);
        lay->addWidget(m_scoreLabel);
        lay->addWidget(m_timerLabel);
    }

public slots:
    void updateLives(int v) { m_livesLabel->setText("Lives: " + QString::number(v)); }
    void updateScore(int v) { m_scoreLabel->setText("Score: " + QString::number(v)); }
    void updateTimer(int s) {
        int m = s / 60, sec = s % 60;
        m_timerLabel->setText(QString("Time: %1:%2")
                                  .arg(m).arg(sec, 2, 10, QChar('0')));
    }
private:
    QLabel* m_livesLabel;
    QLabel* m_scoreLabel;
    QLabel* m_timerLabel;
};
#endif
