#include "pursuerguardian.h"
#include "Artefact.h"
#include "Room.h"
#include "player.h"
#include <QInputDialog>

Pursuerguardian::Pursuerguardian(Room* room, QGraphicsItem* parent)
    : GuardianSprite(GuardianType::PURSUER, parent), m_room(room)
{
    QPixmap px(40, 40);
    px.fill(QColor("#c62828"));
    setPixmap(px);
}

void Pursuerguardian::startEncounter(Player* player)
{
    if (!player || m_triggered)
        return;

    m_triggered = true;
    const QString entered = QInputDialog::getText(nullptr,
                                                  "Pursuer Guardian",
                                                  "Type RUN within 5 seconds:");

    if (entered.trimmed().compare("RUN", Qt::CaseInsensitive) == 0) {
        player->addScore(180);
        player->addArtefact(new Hourglass());
    } else {
        player->loseLife();
    }
    hide();
}
