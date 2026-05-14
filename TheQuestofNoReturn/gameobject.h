#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QRectF>

// Abstract base class for all game objects.
// Defines the interface every game entity must implement.
class GameObject
{
public:
    virtual ~GameObject() = default;

    // Called once per game-loop tick to advance the object's state.
    virtual void update() = 0;

    // Returns the axis-aligned bounding rectangle in scene coordinates.
    virtual QRectF boundingRect() const = 0;

    // Whether the object is still alive/relevant this room.
    virtual bool isActive() const = 0;
};

#endif // GAMEOBJECT_H
