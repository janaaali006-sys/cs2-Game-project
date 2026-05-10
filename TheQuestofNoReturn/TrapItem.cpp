#include "TrapItem.h"
#include "Room.h"
#include "player.h"
#include <QPixmap>
#include <QGraphicsRectItem>
#include <QTimer>
TrapItem::TrapItem(Room* room, QGraphicsItem* parent)
    : GameObject(parent), m_room(room)
{
    QPixmap pixmap(":/images/trap.png");
    if (pixmap.isNull()) {
        pixmap = QPixmap(35, 35);
        pixmap.fill(Qt::red);
    }
    setPixmap(pixmap.scaled(35, 35));
}
void TrapItem::trigger(Player* player)
{
    if (m_triggered)
        return;
    m_triggered = true;
    if (player) {
        player->loseLife();
    }
    flashEffect();
    hide();
}
QString TrapItem::objectName() const
{
    return "TrapItem";
}
void TrapItem::flashEffect()
{
    if (!m_room)
        return;
    QGraphicsRectItem* flash = new QGraphicsRectItem(m_room->sceneRect());
    flash->setBrush(QColor(255, 0, 0, 90));
    flash->setPen(Qt::NoPen);
    flash->setZValue(1000);
    m_room->addItem(flash);
    QTimer::singleShot(200, [this, flash]() {
        if (m_room) {
            m_room->removeItem(flash);
        }
        delete flash;
    });
}
