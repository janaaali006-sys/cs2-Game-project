#include "RiddleAltar.h"

#include "../dialogs/RiddleDialog.h"
#include "../world/RiddleManager.h"
#include "../player.h"

void RiddleAltar::trigger(Player* player) {
    if (m_used) return;
    m_used = true;
    emit riddleTriggered(m_riddle);   // MainWindow/Game will handle showing the dialog
}
