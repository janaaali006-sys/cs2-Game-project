#ifndef DILEMMADIALOG_H
#define DILEMMADIALOG_H

#include <QDialog>

class Player;
class Outcome;

class QPushButton;
class QLabel;

class DilemmaDialog : public QDialog {
    Q_OBJECT

public:
    DilemmaDialog(QString scenario,
                  Outcome* optionA,
                  Outcome* optionB,
                  Outcome* optionC,
                  Player* player,
                  QWidget* parent = nullptr);

private slots:
    void chooseA();
    void chooseB();
    void chooseC();

private:
    void applyOutcome(Outcome* outcome);

    Player* m_player;
    Outcome* m_a;
    Outcome* m_b;
    Outcome* m_c;

    QLabel* m_text;

    QPushButton* m_btnA;
    QPushButton* m_btnB;
    QPushButton* m_btnC;
};

#endif
