#ifndef ENEMY_H
#define ENEMY_H

#include "gameobject.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QPointF>
#include <QtMath>

// ── Abstract enemy base ───────────────────────────────────────────────────────
// Holds all patrol/chase data shared by every enemy type.
// Subclasses differ only in sprite creation and any type-specific animation.

class Enemy : public GameObject
{
public:
    Enemy(QGraphicsScene *scene,
          qreal x, qreal y, qreal w, qreal h,
          bool horiz, float mn, float mx, float spd, float startDir,
          float detectRadius, float chaseSpd);

    ~Enemy() override;

    // GameObject interface
    void   update() override = 0;   // subclasses override for sprite animation
    QRectF boundingRect() const override;
    bool   isActive() const override { return true; }

    // Set player position before each update() call
    void setPlayerPos(QPointF pos) { m_playerPos = pos; }

    // Collision check
    bool collidesWith(QRectF other) const;

protected:
    // Shared patrol/chase logic called by each subclass's update()
    void moveStep();

    QGraphicsRectItem   *m_item;
    QGraphicsPixmapItem *m_sprite;
    bool  m_horiz;
    float m_mn, m_mx, m_spd, m_dir;
    float m_detectionRadius, m_chaseSpeed;
    bool  m_chasing;
    float m_homeX, m_homeY;
    QPointF m_playerPos;
};

// ── Concrete enemy subclasses ─────────────────────────────────────────────────

class Mummy : public Enemy
{
public:
    Mummy(QGraphicsScene *scene,
          qreal x, qreal y, qreal w, qreal h,
          bool horiz, float mn, float mx, float spd, float startDir,
          float detectRadius = 0.f, float chaseSpd = 0.f);

    void update() override;   // patrol/chase; no extra animation
};

class Specter : public Enemy
{
public:
    Specter(QGraphicsScene *scene,
            qreal x, qreal y, qreal w, qreal h,
            bool horiz, float mn, float mx, float spd, float startDir,
            float detectRadius = 0.f, float chaseSpd = 0.f);

    void update() override;   // patrol/chase + opacity flicker

private:
    int m_flickerTick = 0;
};

class Scarab : public Enemy
{
public:
    Scarab(QGraphicsScene *scene,
           qreal x, qreal y, qreal w, qreal h,
           bool horiz, float mn, float mx, float spd, float startDir,
           float detectRadius = 0.f, float chaseSpd = 0.f);

    void update() override;   // patrol/chase; no extra animation
};

class DarkMummy : public Enemy
{
public:
    DarkMummy(QGraphicsScene *scene,
              qreal x, qreal y, qreal w, qreal h,
              bool horiz, float mn, float mx, float spd, float startDir,
              float detectRadius = 0.f, float chaseSpd = 0.f);

    void update() override;   // patrol/chase; no extra animation
};

#endif // ENEMY_H
