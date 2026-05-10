#ifndef GUARDIANSPRITE_H
#define GUARDIANSPRITE_H

#include "GameObject.h"
#include "SharedEnums.h"

class Player;

// GuardianSprite — abstract base for all guardian types.
// Inherits GameObject so it lives in the scene and can trigger on collision.
// Subclasses: WatcherGuardian, PursuerGuardian, RiddlerGuardian
class GuardianSprite : public GameObject
{
    Q_OBJECT
public:
    explicit GuardianSprite(GuardianType type, QGraphicsItem* parent = nullptr);

    virtual void startEncounter(Player* player) = 0;

    void trigger(Player* player) override { startEncounter(player); }
    QString objectName() const override { return "GuardianSprite"; }
    bool isPassable() const override { return false; }

    GuardianType guardianType() const { return m_type; }

protected:
    GuardianType m_type;
};

#endif // GUARDIANSPRITE_H
