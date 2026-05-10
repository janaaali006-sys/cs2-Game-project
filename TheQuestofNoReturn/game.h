#ifndef GAME_H
#define GAME_H
#include <QObject>
#include "player.h"
#include "riddlegenerator.h"
#include "SharedEnums.h"

class Game : public QObject {
    Q_OBJECT
public:
    explicit Game(QObject* parent = nullptr);
    Player*         getPlayer()         { return m_player; }
    RiddleGenerator* getRiddleGenerator(){ return m_riddleGen; }
    void startNewGame();
    void endGame(bool won);
    int  getCurrentLevel() const { return m_currentLevel; }

signals:
    void gameStarted();
    void gameEnded(bool won);
    void levelChanged(int level);

private:
    Player*          m_player    = nullptr;
    RiddleGenerator* m_riddleGen = nullptr;
    int              m_currentLevel = 1;
};
#endif
