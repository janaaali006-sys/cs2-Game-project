#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <QString>
#include "SharedEnums.h"

// ── Enums ─────────────────────────────────────────────────────────────────────

enum class BossState { INACTIVE, SHOOTING, TIRED, DYING };

enum class RoomMechanic {
    NONE,
    TUTORIAL,
    STEALTH_GUARDIAN,
    FAKE_DOORS,
    SPIKE_TRAPS,
    HIDDEN_RELIC_PUZZLE,
    BOSS_FIGHT,
    COLLAPSING_FLOOR
};

// ── Room narrative data ───────────────────────────────────────────────────────

struct RoomInfo {
    QString name;
    QString enterText;
    QString solvedText;
    bool    survivorHeals;
    int     baseScore;
    int     bonusScore;
};

extern const RoomInfo ROOMS[7];

// ── Per-session player data ───────────────────────────────────────────────────

struct PlayerData {
    QString    character   = "Zara";
    QString    trait       = "Scholar";
    QString    name        = "";
    Difficulty difficulty  = Difficulty::MEDIUM;
};

// ── Mutable game state (no UI, no graphics pointers) ─────────────────────────

struct GameState {
    int  timeSeconds     = 0;
    int  lives           = 0;
    int  maxLives        = 0;
    int  score           = 0;
    int  currentRoom     = 0;
    bool gameActive      = false;
    bool invincible      = false;
    int  invFrames       = 0;
    bool riddleOpen      = false;
    int  animTick        = 0;
    int  animFrame       = 0;

    int          cluesFound      = 0;
    int          requiredClues   = 3;
    int          relicsCollected = 0;
    int          totalRelics     = 7;
    int          spikeTimer      = 0;
    RoomMechanic currentMechanic = RoomMechanic::NONE;

    // Boss sub-state
    BossState bossState        = BossState::INACTIVE;
    int       bossHP           = 0;
    int       bossShootCount   = 0;
    int       bossShootTimer   = 0;
    int       bossTiredTimer   = 0;
    int       bossHitsThisPhase= 0;
    int       bossDyingTimer   = 0;
};

#endif // GAMEDATA_H
