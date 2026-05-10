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
    ZARA,   // Dr. Zara Khalil
    KARIM,  // Dr. Karim Hassan
    NOUR,   // Dr. Nour Farouk
    RAMI    // Dr. Rami Saad
};

enum class RiddleType {
    IDENTITY,   // "Who am I?" format
    LOGIC,      // reasoning / deduction
    SEQUENCE    // pattern / order completion
};

enum class RoomMood {
    CALM,    // standard gameplay
    CURSED,  // decoy answer added, -15s on wrong answer
    BLESSED  // first-try correct answer = bonus artefact drop
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
    WATCHER,   // strategic choice
    PURSUER,   // reflex keypress
    RIDDLER    // hard riddle
};

#endif // SHAREDENUMS_H
