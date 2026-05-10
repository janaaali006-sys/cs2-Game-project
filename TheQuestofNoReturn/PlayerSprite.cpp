#include "PlayerSprite.h"
#include "GameObject.h"
#include "player.h"
#include <QGraphicsScene>
#include <QPixmap>
PlayerSprite::PlayerSprite(Player* player, QGraphicsItem* parent)
    : QObject(), QGraphicsPixmapItem(parent), m_player(player)
{
    QPixmap pixmap(":/images/player.png");
    if (pixmap.isNull()) {
        pixmap = QPixmap(40, 40);
        pixmap.fill(Qt::blue);
    }
    setPixmap(pixmap.scaled(40, 40));
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}
Player* PlayerSprite::playerData() const
{
    return m_player;
}
void PlayerSprite::keyPressEvent(QKeyEvent* event)
{
    if (!scene())
        return;
    QPointF oldPos = pos();
    if (event->key() == Qt::Key_Left) {
        setX(x() - m_speed);
    } else if (event->key() == Qt::Key_Right) {
        setX(x() + m_speed);
    } else if (event->key() == Qt::Key_Up) {
        setY(y() - m_speed);
    } else if (event->key() == Qt::Key_Down) {
        setY(y() + m_speed);
    }
    QRectF bounds = scene()->sceneRect();
    if (!bounds.contains(sceneBoundingRect())) {
        setPos(oldPos);
    }
}
void PlayerSprite::advance(int phase)
{
    if (phase == 0 || !scene() || !m_player)
        return;
    QList<QGraphicsItem*> collisions = scene()->collidingItems(this);
    for (QGraphicsItem* item : collisions) {
        GameObject* object = dynamic_cast<GameObject*>(item);
        if (object && !object->isTriggered()) {
            object->trigger(m_player);
        }
    }
}
