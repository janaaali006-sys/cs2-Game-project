#include "riddlerguardian.h"
#include "Artefact.h"
#include "Room.h"
#include "dialogs/RiddleDialog.h"
#include "player.h"
#include "riddlegenerator.h"
#include "config.h"

Riddlerguardian::Riddlerguardian(Room* room, QGraphicsItem* parent)
    : GuardianSprite(GuardianType::RIDDLER, parent), m_room(room)
{
    QPixmap px(40, 40);
    px.fill(QColor("#1565c0"));
    setPixmap(px);
}

void Riddlerguardian::startEncounter(Player* player)
{
    if (!player || m_triggered)
        return;
    m_triggered = true;

    RiddleGenerator generator;
    generator.setApiKey(GOOGLEAI_API_KEY);
    Riddle* guardianRiddle = generator.generate("Guardian",
                                                Difficulty::HARD,
                                                RiddleType::LOGIC,
                                                RoomMood::CURSED);
    if (!guardianRiddle) {
        player->loseLife();
        hide();
        return;
    }

    RiddleDialog dialog(guardianRiddle, player, nullptr);
    if (dialog.exec() == QDialog::Accepted) {
        player->addScore(220);
        player->addArtefact(new ScrollOfAnubis());
    } else {
        player->loseLife();
    }
    delete guardianRiddle;
    hide();
}
