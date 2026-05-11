#ifndef RIDDLEDIALOG_H
#define RIDDLEDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QCloseEvent>

class Riddle;
class Player;
class RiddleManager;

class QLabel;
class QLineEdit;
class QPushButton;
class QProgressBar;

class RiddleDialog : public QDialog {
    Q_OBJECT

public:
    RiddleDialog(Riddle* riddle, Player* player, QWidget* parent = nullptr);

    void setManager(RiddleManager* manager);

protected:
    void closeEvent(QCloseEvent* event) override;
    void reject() override;

private slots:
    void onSubmit();
    void updateTimer();
    void showHint();

private:
    void setupUI();
    void fail();

    Riddle* m_riddle;
    Player* m_player;
    RiddleManager* m_manager = nullptr;

    QLabel* m_promptLabel;
    QLabel* m_hintLabel;
    QLineEdit* m_input;
    QPushButton* m_submit;
    QPushButton* m_hintBtn;
    QProgressBar* m_timerBar;

    QTimer m_timer;
    int m_timeLeft = 0;
    bool m_finished = false;
};

#endif
