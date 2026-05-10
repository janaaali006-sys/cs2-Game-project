#include "Guardiansprite.h"

GuardianSprite::GuardianSprite(GuardianType type, QGraphicsItem* parent)
    : GameObject(parent), m_type(type)
{
    QPixmap px(40, 40);
    px.fill(Qt::darkRed);
    setPixmap(px);
}
