#ifndef SHAREDENUMS_H
#define SHAREDENUMS_H

enum class Difficulty {
    EASY,    // 3 lives, 12 min timer, 45s riddle timer
    MEDIUM,  // 2 lives, 8 min timer,  30s riddle timer
    HARD     // 1 life,  5 min timer,  20s riddle timer
};

enum class CharacterTrait {
    SCHOLAR,    // hint available once per level
    SURVIVOR,   // +1 life at game start
    SPEEDSTER   // +30 seconds on global timer
};

enum class CharacterID {
    ZARA,
    KARIM,
    NOUR,
    RAMI
};

enum class RiddleType {
    IDENTITY,
    LOGIC,
    SEQUENCE
};

enum class RoomMood {
    CALM,
    CURSED,
    BLESSED
};

enum class ArtefactType {
    EYE_OF_HORUS,
    HOURGLASS,
    SCARAB_OF_RA,
    SCROLL_OF_ANUBIS,
    BLADE_OF_SEKHMET
};

enum class GameState {
    MENU,
    CHARACTER_SETUP,
    PLAYING,
    PAUSED,
    GAMEOVER,
    WIN
};

enum class GuardianType {
    WATCHER,
    PURSUER,
    RIDDLER
};

#endif // SHAREDENUMS_H
