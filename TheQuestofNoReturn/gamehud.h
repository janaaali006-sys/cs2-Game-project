#ifndef GAMEHUD_H
#define GAMEHUD_H

#include <QString>
#include <QLabel>

// Forward-declare the generated UI class to avoid pulling in the full header here.
namespace Ui { class MainWindow; }

// Encapsulates all HUD (heads-up display) update logic.
// No game logic or game-object data lives here — only widget manipulation.
class GameHUD
{
public:
    // mapLabels must be a pointer to an array of 6 QLabel pointers.
    GameHUD(Ui::MainWindow *ui, QLabel **mapLabels);

    void refreshTimer(int timeSeconds);
    void refreshLives(int lives);
    void refreshMap(int currentRoom);
    void setEventText(const QString &msg);
    void setScore(int score);

private:
    Ui::MainWindow *m_ui;
    QLabel        **m_mapLabels;   // points into MainWindow's m_mapLabels array
};

#endif // GAMEHUD_H
