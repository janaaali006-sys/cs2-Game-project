#include "RiddleAltar.h"
#include "../Room.h"
#include "../dialogs/RiddleDialog.h"
#include "../world/RiddleManager.h"
#include "../player.h"

// RiddleAltar::trigger — called by PlayerSprite::advance() on collision.
// Shows the riddle dialog. On success, marks the room as solved
// (which unlocks the door). On fail, player loses a life.
void RiddleAltar::trigger(Player* player) {
    if (m_used) return;
    m_used = true;

    // Show the dialog — RiddleDialog handles correct/wrong answer logic
    // and calls m_manager->onRiddleSuccess / onRiddleFail internally.
    // We pass nullptr for manager here; MainWindow wires it via setRiddle flow.
    RiddleDialog dialog(m_riddle, player, nullptr);

    // Connect: if dialog accepted (correct answer) → mark room solved
    if (dialog.exec() == QDialog::Accepted) {
        if (m_room) {
            m_room->setRiddleSolved(true);
        }
    }
}
