#include "Game.h"
#include "RoomManager.h"
#include "player.h"
#include "savemanager.h"

Game::Game(QGraphicsView* view, QObject* parent)
    : QObject(parent), m_view(view)
{
}

Game::~Game()
{
    delete m_roomManager;
    delete m_player;
}

GameState Game::state() const { return m_state; }

void Game::startNewGame(Difficulty difficulty,
                        CharacterTrait trait,
                        CharacterID characterID,
                        const QString& name,
                        const QString& avatarPath)
{
    // Clean up any previous game
    delete m_roomManager; m_roomManager = nullptr;
    delete m_player;      m_player      = nullptr;

    // Store for restartGame()
    m_difficulty   = difficulty;
    m_trait        = trait;
    m_characterID  = characterID;
    m_playerName   = name;
    m_avatarPath   = avatarPath;

    // Create player — our Player class sets lives/timer from difficulty+trait
    m_player = new Player(this);
    m_player->setCharacterName(name);
    m_player->setCharacterID(characterID);
    m_player->setTrait(trait);
    m_player->setAvatarPath(avatarPath);
    m_player->setDifficulty(difficulty);  // calls applyDifficultyDefaults()
    m_player->reset();
    m_player->startTimer();

    // Create room manager — it owns the scene/view
    m_roomManager = new RoomManager(m_view, m_player, this);

    // Wire signals
    connect(m_player, &Player::gameOver, this, [this]() {
        endGame(false);
    });
    connect(m_player, &Player::timerExpired, this, [this]() {
        endGame(false);
    });
    connect(m_roomManager, &RoomManager::gameWon, this, [this]() {
        endGame(true);
    });

    setState(GameState::PLAYING);
    m_roomManager->startGame();
}

void Game::pauseGame()
{
    if (m_state != GameState::PLAYING) return;
    if (m_player) m_player->pauseTimer();
    setState(GameState::PAUSED);
}

void Game::resumeGame()
{
    if (m_state != GameState::PAUSED) return;
    if (m_player) m_player->resumeTimer();
    setState(GameState::PLAYING);
}

void Game::restartGame()
{
    startNewGame(m_difficulty, m_trait, m_characterID, m_playerName, m_avatarPath);
}

void Game::endGame(bool won)
{
    if (m_player) m_player->pauseTimer();

    // Save state when game ends
    int level = m_roomManager ? m_roomManager->currentLevel() : 1;
    if (m_player) SaveManager::saveGame(m_player, level);

    setState(won ? GameState::WIN : GameState::GAMEOVER);
    emit gameEnded(won);
}

void Game::transitionTo(GameState state)
{
    // Keep state transitions explicit and centralized.
    if (state == GameState::PLAYING) {
        if (m_state == GameState::PAUSED) {
            resumeGame();
        } else {
            if (m_player) m_player->resumeTimer();
            setState(GameState::PLAYING);
        }
        return;
    }
    if (state == GameState::PAUSED) {
        pauseGame();
        return;
    }
    setState(state);
}

Player*      Game::player()       const { return m_player; }
RoomManager* Game::roomManager()  const { return m_roomManager; }
int          Game::currentLevel() const {
    return m_roomManager ? m_roomManager->currentLevel() : 0;
}

void Game::setState(GameState state)
{
    if (m_state == state) return;
    m_state = state;
    emit stateChanged(m_state);
}
