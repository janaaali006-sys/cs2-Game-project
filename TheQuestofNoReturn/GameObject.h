#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QString>

class Player;

// ============================================================
// GameObject — abstract base class for every visible object
// in the game scene (door, trap, altar, artefact item...).
//
// OOP role: Abstraction + Polymorphism + Inheritance
//   - Inherits QObject (signals/slots) + QGraphicsPixmapItem
//     (renders as a sprite in the QGraphicsScene).
//   - trigger() and objectName() are pure virtual — every
//     subclass must define its own behaviour.
//   - PlayerSprite::advance() calls trigger(player) on whatever
//     it collides with, without knowing the concrete type.
// ============================================================
class GameObject : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit GameObject(QGraphicsItem* parent = nullptr);
    virtual ~GameObject();

    // Called by PlayerSprite on collision
    virtual void trigger(Player* player) = 0;

    // True = player can walk through (e.g. open door)
    virtual bool isPassable() const;

    // Human-readable name for debugging
    virtual QString objectName() const = 0;

    // Prevents trigger() from firing more than once
    bool isTriggered() const;

protected:
    bool m_triggered = false;
};

#endif // GAMEOBJECT_H
