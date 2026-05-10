#include "GameObject.h"

GameObject::GameObject(QGraphicsItem* parent)
    : QObject(), QGraphicsPixmapItem(parent)
{
}

GameObject::~GameObject()
{
}

bool GameObject::isPassable() const
{
    return false;
}

bool GameObject::isTriggered() const
{
    return m_triggered;
}
