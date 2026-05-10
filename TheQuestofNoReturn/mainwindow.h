#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Game.h"
#include "avatargenerator.h"
#include "savemanager.h"
#include "SharedEnums.h"

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
    void onGameEnded(bool won);
    void onAvatarReady(QString path);

private:
    Ui::MainWindow*  ui;
    Game*            m_game      = nullptr;
    AvatarGenerator* m_avatarGen = nullptr;

    QString    m_selectedCharacter  = "Zara";
    QString    m_selectedTrait      = "Scholar";
    Difficulty m_selectedDifficulty = Difficulty::MEDIUM;

    void setupCharacterButtons();
    void setupConfirmButton();
    void setupMenuButtons();
    void setupHUDButtons();
    void setupAvatarGenerator();
    void setupPlayerHUDSignals();
    void startGame();
    void updateHUDDefaults();
};

#endif // MAINWINDOW_H
