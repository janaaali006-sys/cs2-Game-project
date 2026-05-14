#ifndef INTERACTABLE_H
#define INTERACTABLE_H

#include "gameobject.h"
#include <QString>
#include <QList>
#include <QPointF>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QtMath>

// ── Result returned by every Interactable::interact() call ───────────────────
// MainWindow reads this and applies it to shared game state.

struct InteractResult {
    QString eventText;
    int     scoreChange    = 0;
    bool    clueCollected  = false;   // ClueFragmentObj
    bool    relicCollected = false;   // RelicObj
    bool    altarReady     = false;   // AltarObj: MainWindow calls showRiddle()
    bool    torchActivated = false;   // TorchObj lit: MainWindow reveals hidden relics
};

// ── Abstract interactable base ────────────────────────────────────────────────

class Interactable : public GameObject
{
public:
    // Default: static objects — subclasses override if they animate
    void update() override {}
    bool isActive() const override { return !m_done; }

    // Pure virtual: each subclass defines its own interaction effect
    virtual InteractResult interact() = 0;

    // Virtual: returns true when player is close enough; default uses m_center + radius
    virtual bool isNear(QPointF playerPos, float radius = 70.f) const;

    // Default: no bounding rect needed for most interactables
    QRectF boundingRect() const override { return {}; }

    // Store player position; used by AltarObj::update() to show/hide hint
    void setPlayerPos(QPointF pos) { m_playerPos = pos; }

    // Called by MainWindow when a torch is activated (revealable objects respond)
    virtual void reveal() {}

protected:
    bool    m_done   = false;
    QPointF m_center;          // used by default isNear()
    QPointF m_playerPos;

    static float qdist(QPointF a, QPointF b)
    {
        float dx = float(a.x() - b.x()), dy = float(a.y() - b.y());
        return qSqrt(dx * dx + dy * dy);
    }
};

// ── ClueFragmentObj ───────────────────────────────────────────────────────────

class ClueFragmentObj : public Interactable
{
public:
    ClueFragmentObj(qreal x, qreal y, QGraphicsScene *scene);
    ~ClueFragmentObj() override;

    void update() override;             // animates the golden glow
    InteractResult interact() override; // hides sprite + glow; returns clueCollected=true
    bool isNear(QPointF pos, float radius = 70.f) const override;

private:
    QGraphicsPixmapItem *m_sprite  = nullptr;
    QGraphicsRectItem   *m_glow    = nullptr;
    float                m_glowPhase = 0.f;
};

// ── StatueObj ─────────────────────────────────────────────────────────────────

class StatueObj : public Interactable
{
public:
    StatueObj(qreal x, qreal y, const QString &lore, QGraphicsScene *scene);
    ~StatueObj() override;

    InteractResult interact() override; // returns lore text as eventText

private:
    QGraphicsPixmapItem *m_sprite = nullptr;
    QGraphicsRectItem   *m_hitbox = nullptr;
    QString              m_lore;
};

// ── TabletObj ─────────────────────────────────────────────────────────────────

class TabletObj : public Interactable
{
public:
    TabletObj(qreal x, qreal y, const QString &hint, QGraphicsScene *scene);
    ~TabletObj() override;

    InteractResult interact() override; // returns hint text as eventText

private:
    QGraphicsPixmapItem *m_sprite = nullptr;
    QGraphicsRectItem   *m_hitbox = nullptr;
    QString              m_hint;
};

// ── TorchObj ──────────────────────────────────────────────────────────────────

class TorchObj : public Interactable
{
public:
    TorchObj(qreal x, qreal y, QGraphicsScene *scene,
             QList<QGraphicsItem*> reveals = {});
    ~TorchObj() override;

    InteractResult interact() override; // toggles flame; returns torchActivated flag

private:
    QGraphicsPixmapItem   *m_sprite   = nullptr;
    QGraphicsRectItem     *m_hitbox   = nullptr;
    bool                   m_activated = false;
    QList<QGraphicsItem*>  m_reveals;
};

// ── RelicObj ──────────────────────────────────────────────────────────────────

class RelicObj : public Interactable
{
public:
    RelicObj(qreal x, qreal y, bool startVisible, QGraphicsScene *scene);
    ~RelicObj() override;

    bool isActive() const override { return m_visible && !m_done; }
    InteractResult interact() override; // hides sprite; returns relicCollected=true, +50 score
    bool isNear(QPointF pos, float radius = 70.f) const override;

    void reveal() override; // called by MainWindow when torch is lit

private:
    QGraphicsPixmapItem *m_sprite  = nullptr;
    bool                 m_visible = false;
};

// ── AltarObj ──────────────────────────────────────────────────────────────────

class AltarObj : public Interactable
{
public:
    AltarObj(qreal x, qreal y, QGraphicsScene *scene);
    ~AltarObj() override;

    void update() override;             // pulses glow; shows/hides hint near player
    bool isActive() const override { return m_active; }
    InteractResult interact() override; // returns altarReady=true
    bool isNear(QPointF pos, float radius = 100.f) const override;

    void activate(QGraphicsScene *scene); // called by MainWindow when enough clues collected

private:
    QGraphicsPixmapItem *m_sprite     = nullptr;
    QGraphicsRectItem   *m_hitbox     = nullptr;
    QGraphicsRectItem   *m_glowEffect = nullptr;
    QGraphicsTextItem   *m_hint       = nullptr;
    bool                 m_active     = false;
    float                m_glowPhase  = 0.f;
};

#endif // INTERACTABLE_H
