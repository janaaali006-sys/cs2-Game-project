#include "Game.h"
#include "savemanager.h"
#include "config.h"

Game::Game(QObject* parent) : QObject(parent) {
    m_player    = new Player(this);
    m_riddleGen = new RiddleGenerator(this);
    m_riddleGen->setApiKey(CLAUDE_API_KEY);

    // Wire player signals
    connect(m_player, &Player::gameOver,     this, [this]{ endGame(false); });
    connect(m_player, &Player::timerExpired, this, [this]{ endGame(false); });
}

void Game::startNewGame() {
    m_currentLevel = 1;
    m_player->reset();
    m_player->startTimer();
    emit gameStarted();
    emit levelChanged(m_currentLevel);

    // In Game.cpp — generate a test riddle and show it works
    Riddle* r = m_riddleGen->generate("Ra", m_player->getDifficulty(),
                                      RiddleType::IDENTITY, RoomMood::CALM);
    // Store it for when RiddleAltar::trigger() emits riddleTriggered
    m_currentRiddle = r;
}

void Game::endGame(bool won) {
    m_player->pauseTimer();
    emit gameEnded(won);
}
