#include "DoorItem.h"
#include "Room.h"
#include <QPixmap>
DoorItem::DoorItem(Room* room, QGraphicsItem* parent)
    : GameObject(parent), m_room(room)
{
    QPixmap pixmap(":/images/door.png");
    if (pixmap.isNull()) {
        pixmap = QPixmap(50, 80);
        pixmap.fill(Qt::darkYellow);
    }
    setPixmap(pixmap.scaled(50, 80));
}
void DoorItem::trigger(Player* player)
{
    Q_UNUSED(player);
    if (!m_room)
        return;
    if (!m_room->isRiddleSolved()) {
        return;
    }
    m_triggered = true;
    m_room->completeRoom();
}
bool DoorItem::isPassable() const
{
    return true;
}
QString DoorItem::objectName() const
{
    return "DoorItem";
}
