#include "RiddleAltar.h"

#include "../dialogs/RiddleDialog.h"
#include "../world/RiddleManager.h"
#include "../entities/Player.h"

void RiddleAltar::trigger(Player* player) {

    if (m_used) return;
    m_used = true;

    // IMPORTANT: player must already own a RiddleManager
    RiddleManager* manager = player->getRiddleManager();

    RiddleDialog dialog(m_riddle, player);
    dialog.setManager(manager);

    dialog.exec();
}
