#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "player.h"
#include "riddlegenerator.h"
#include "avatargenerator.h"
#include "savemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startButton_clicked();
    void onGameOver();          // called when player dies or timer runs out
    void onAvatarReady(QString path);

private:
    Ui::MainWindow* ui;

    // Core objects — created once in the constructor, live for the whole app
    Player*          m_player    = nullptr;
    RiddleGenerator* m_riddleGen = nullptr;
    AvatarGenerator* m_avatarGen = nullptr;

    // Selected values from the character setup screen
    // (keep these — the character/trait buttons still set them)
    QString          m_selectedCharacter = "Zara";
    QString          m_selectedTrait     = "Scholar";
    Difficulty       m_selectedDifficulty = Difficulty::MEDIUM;

    void setupPlayerSignals();  // wires player → UI updates
    void startGame();           // called after confirm button passes validation
};

#endif // MAINWINDOW_H
