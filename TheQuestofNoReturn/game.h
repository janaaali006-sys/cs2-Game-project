#pragma once
#include <QObject>
#include <QString>
#include <QGraphicsView>
#include "SharedEnums.h"

class Player;
class RoomManager;

// ============================================================
// Game — top-level controller that owns Player and RoomManager.
// MainWindow creates one Game object and interacts with it.
// ============================================================
class Game : public QObject
{
    Q_OBJECT
public:
    explicit Game(QGraphicsView* view, QObject* parent = nullptr);
    ~Game();

    GameState state() const;

    // Called from MainWindow's confirm button
    void startNewGame(Difficulty difficulty,
                      CharacterTrait trait,
                      CharacterID characterID,
                      const QString& name,
                      const QString& avatarPath);

    void pauseGame();
    void resumeGame();
    void restartGame();
    void endGame(bool won);

    Player*      player()      const;
    RoomManager* roomManager() const;
    int          currentLevel()const;

signals:
    void stateChanged(GameState state);
    void gameEnded(bool won);

private:
    void setState(GameState state);

    GameState      m_state       = GameState::MENU;
    QGraphicsView* m_view        = nullptr;
    Player*        m_player      = nullptr;
    RoomManager*   m_roomManager = nullptr;

    // Stored for restartGame()
    Difficulty     m_difficulty  = Difficulty::MEDIUM;
    CharacterTrait m_trait       = CharacterTrait::SCHOLAR;
    CharacterID    m_characterID = CharacterID::ZARA;
    QString        m_playerName;
    QString        m_avatarPath;
};
