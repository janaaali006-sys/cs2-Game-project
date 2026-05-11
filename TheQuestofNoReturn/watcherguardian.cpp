#include "watcherguardian.h"
#include "Artefact.h"
#include "Room.h"
#include "player.h"
#include <QMessageBox>

Watcherguardian::Watcherguardian(Room* room, QGraphicsItem* parent)
    : GuardianSprite(GuardianType::WATCHER, parent), m_room(room)
{
    QPixmap px(40, 40);
    px.fill(QColor("#7b1fa2"));
    setPixmap(px);
}

void Watcherguardian::startEncounter(Player* player)
{
    if (!player || m_triggered)
        return;

    m_triggered = true;
    const auto result = QMessageBox::question(nullptr,
                                              "Watcher Guardian",
                                              "A silent watcher blocks your path.\nObserve and stand your ground?",
                                              QMessageBox::Yes | QMessageBox::No,
                                              QMessageBox::Yes);

    if (result == QMessageBox::Yes || player->hasArtefact(ArtefactType::SCARAB_OF_RA)) {
        player->addScore(150);
        player->addArtefact(new EyeOfHorus());
        hide();
        return;
    }

    player->loseLife();
    hide();
}
