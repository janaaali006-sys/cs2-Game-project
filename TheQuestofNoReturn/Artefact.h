#ifndef ARTEFACT_H
#define ARTEFACT_H

#include <QString>
#include "SharedEnums.h"

class Player;

// ============================================================
// Artefact — abstract base class for all collectible items.
//
// OOP role: Inheritance + Polymorphism + Abstraction
//   - Each artefact subclass overrides apply() with its own
//     unique effect on the Player.
//   - Player::addArtefact() stores Artefact* — it never needs
//     to know which specific artefact it holds.
//   - Follows the same abstract-interface pattern as Outcome
//     and GameObject.
// ============================================================
class Artefact {
public:
    virtual ~Artefact() {}

    // Apply this artefact's effect to the player
    virtual void apply(Player* player) = 0;

    // One-line description shown in the HUD / revelation dialog
    virtual QString getDescription() const = 0;

    // Returns the enum type so Player can query the inventory
    virtual ArtefactType getType() const = 0;

    // Display name (e.g. "Eye of Horus")
    virtual QString getName() const = 0;
};

// ============================================================
// Concrete artefact subclasses
// Each overrides apply() with its specific game effect.
// ============================================================

// EyeOfHorus — reveals the answer hint immediately
class EyeOfHorus : public Artefact {
public:
    void apply(Player* player) override;
    QString getDescription() const override { return "Reveals the riddle hint instantly."; }
    ArtefactType getType() const override   { return ArtefactType::EYE_OF_HORUS; }
    QString getName() const override        { return "Eye of Horus"; }
};

// Hourglass — adds 60 seconds to the global timer
class Hourglass : public Artefact {
public:
    void apply(Player* player) override;
    QString getDescription() const override { return "Adds 60 seconds to your remaining time."; }
    ArtefactType getType() const override   { return ArtefactType::HOURGLASS; }
    QString getName() const override        { return "Hourglass"; }
};

// ScarabOfRa — absorbs the next life loss
class ScarabOfRa : public Artefact {
public:
    void apply(Player* player) override;
    QString getDescription() const override { return "Shields you from the next life loss."; }
    ArtefactType getType() const override   { return ArtefactType::SCARAB_OF_RA; }
    QString getName() const override        { return "Scarab of Ra"; }
};

// ScrollOfAnubis — grants +200 score bonus
class ScrollOfAnubis : public Artefact {
public:
    void apply(Player* player) override;
    QString getDescription() const override { return "Grants 200 bonus points from the god of the dead."; }
    ArtefactType getType() const override   { return ArtefactType::SCROLL_OF_ANUBIS; }
    QString getName() const override        { return "Scroll of Anubis"; }
};

// BladeOfSekhmet — no immediate effect; used as a score multiplier at game end
class BladeOfSekhmet : public Artefact {
public:
    void apply(Player* player) override;
    QString getDescription() const override { return "Doubles your final score at game end."; }
    ArtefactType getType() const override   { return ArtefactType::BLADE_OF_SEKHMET; }
    QString getName() const override        { return "Blade of Sekhmet"; }
};

#endif // ARTEFACT_H
