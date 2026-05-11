#include "RiddleAltar.h"
#include "../dialogs/RiddleDialog.h"
#include "../dialogs/DilemmaDialog.h"
#include "../world/RiddleManager.h"
#include "../player.h"
#include "../Room.h"
#include "../outcomes/GoodOutcome.h"
#include "../outcomes/LoseLifeOutcome.h"
#include "../outcomes/HintOutcome.h"

// RiddleAltar::trigger — called by PlayerSprite::advance() on collision.
// Shows the riddle dialog. On success, marks the room as solved
// (which unlocks the door). On fail, player loses a life.
void RiddleAltar::trigger(Player* player) {
    if (m_used) return;

    // Show the dialog — RiddleDialog handles correct/wrong answer logic
    // and calls m_manager->onRiddleSuccess / onRiddleFail internally.
    // We pass nullptr for manager here; MainWindow wires it via setRiddle flow.
    RiddleDialog dialog(m_riddle, player, nullptr);

    // Connect: if dialog accepted (correct answer) → mark room solved
    if (dialog.exec() == QDialog::Accepted) {
        m_used = true;
        if (m_room) {
            m_room->setRiddleSolved(true);
        }

        // Dilemma phase directly after successful riddle.
        GoodOutcome good("You made a wise sacrifice. The chamber accepts your judgment.", 120);
        LoseLifeOutcome bad("The chamber rejects your choice.");
        HintOutcome clue("A hidden carving reveals a clue for the next trial.");

        const QString scenario =
            "A cracked altar asks for an offering.\n"
            "Choose: Scarab (A), Memory shard (B), or Ancient map fragment (C).";
        DilemmaDialog dilemma(scenario, &good, &bad, &clue, player, nullptr);
        dilemma.exec();
    }
}
