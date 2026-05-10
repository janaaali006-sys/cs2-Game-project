#include "Artefact.h"
#include "player.h"

// EyeOfHorus — makes hint available immediately
void EyeOfHorus::apply(Player* player) {
    player->setHintAvailable(true);
}

// Hourglass — adds 60 seconds to the timer
void Hourglass::apply(Player* player) {
    // Access current seconds and add 60
    int newTime = player->getSecondsRemaining() + 60;
    // We update via the player's internal state directly
    // (Player exposes setSecondsRemaining for artefact use)
    player->addSeconds(60);
}

// ScarabOfRa — enables the shield that blocks one life loss
void ScarabOfRa::apply(Player* player) {
    player->setScarabShield(true);
}

// ScrollOfAnubis — immediate score bonus
void ScrollOfAnubis::apply(Player* player) {
    player->addScore(200);
}

// BladeOfSekhmet — marks the player for end-game score doubling
// The actual doubling is handled in Game::endGame() by checking hasArtefact()
void BladeOfSekhmet::apply(Player* /*player*/) {
    // Effect is passive — checked at game end via player->hasArtefact()
}
