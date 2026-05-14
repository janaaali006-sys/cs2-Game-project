#include "interactable.h"
#include <QPainter>
#include <QFont>

// ── Sprite creators (file-scope; identical pixel art to original) ─────────────

static QPixmap createClueSprite(int w = 28, int h = 28)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    p.setBrush(QColor(255, 210, 80, 60));
    p.setPen(Qt::NoPen);
    p.drawEllipse(2, 2, w - 4, h - 4);

    QPolygonF crystal;
    crystal << QPointF(w / 2, 4)
            << QPointF(w - 6, h / 2)
            << QPointF(w / 2, h - 4)
            << QPointF(6, h / 2);
    p.setBrush(QColor(255, 200, 60, 230));
    p.setPen(QPen(QColor(255, 240, 150), 2));
    p.drawPolygon(crystal);

    p.fillRect(w / 2 - 2, h / 3, 4, 4, QColor(255, 255, 200, 200));
    p.end();
    return pix;
}

static QPixmap createStatueSprite(int w = 40, int h = 80)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    QColor stone(120, 100, 70, 240);
    QColor dark(80, 65, 45, 230);
    QColor gold(200, 165, 50, 220);

    p.fillRect(2, h - 12, w - 4, 12, dark);
    p.fillRect(4, h - 14, w - 8, 4, stone);

    int bodyH = h - 20;
    p.fillRect(w / 4, 20, w / 2, bodyH - 6, stone);
    p.fillRect(w / 4 - 4, 6, w / 2 + 8, 18, stone);
    p.fillRect(w / 4, 0, w / 2, 10, gold);
    p.fillRect(w / 2 - 4, 12, 3, 2, dark);
    p.fillRect(w / 2 + 2, 12, 3, 2, dark);
    p.fillRect(w / 2 - 2, 18, 5, 2, dark);
    p.fillRect(w / 4 - 2, 28, w / 2 + 4, 6, stone);
    p.fillRect(w / 4, 32, 6, 20, stone);
    p.fillRect(w - w / 4 - 6, 32, 6, 20, stone);
    p.fillRect(w / 4, 26, w / 2, 2, gold);
    p.fillRect(w / 4, 45, w / 2, 2, gold);

    p.end();
    return pix;
}

static QPixmap createTabletSprite(int w = 50, int h = 40)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    QColor stone(160, 140, 100, 240);
    QColor dark(100, 85, 55, 230);
    QColor glyph(60, 45, 25, 200);

    p.fillRect(2, 4, w - 4, h - 6, stone);
    p.fillRect(0, 2, w, 4, dark);
    p.fillRect(0, h - 4, w, 4, dark);

    int glyphY = 12;
    for (int row = 0; row < 2; row++) {
        for (int col = 0; col < 4; col++) {
            int gx = 8 + col * 10;
            int gy = glyphY + row * 12;
            if ((row + col) % 3 == 0)
                p.fillRect(gx, gy, 6, 8, glyph);
            else if ((row + col) % 3 == 1) {
                p.fillRect(gx, gy, 8, 3, glyph);
                p.fillRect(gx + 2, gy + 3, 4, 5, glyph);
            } else {
                p.fillRect(gx + 2, gy, 4, 8, glyph);
                p.fillRect(gx, gy + 2, 8, 3, glyph);
            }
        }
    }
    p.end();
    return pix;
}

static QPixmap createTorchSprite(int w = 20, int h = 45, bool lit = false)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    QColor handle(90, 60, 30, 245);
    QColor metal(140, 120, 80, 240);

    p.fillRect(w / 2 - 3, h / 3, 6, h * 2 / 3, handle);
    p.fillRect(w / 2 - 6, h / 3, 12, 4, metal);
    p.fillRect(w / 2 - 8, h / 3 - 2, 4, 8, metal);
    p.fillRect(w / 2 - 7, 8, 14, 10, metal);

    if (lit) {
        p.setBrush(QColor(255, 180, 40, 200));
        p.setPen(Qt::NoPen);
        QPolygonF flame;
        flame << QPointF(w / 2, 0)
              << QPointF(w / 2 + 6, 12)
              << QPointF(w / 2, 8)
              << QPointF(w / 2 - 6, 12);
        p.drawPolygon(flame);
        p.setBrush(QColor(255, 240, 120, 220));
        QPolygonF inner;
        inner << QPointF(w / 2, 3)
              << QPointF(w / 2 + 3, 10)
              << QPointF(w / 2, 7)
              << QPointF(w / 2 - 3, 10);
        p.drawPolygon(inner);
    }
    p.end();
    return pix;
}

static QPixmap createRelicSprite(int w = 32, int h = 24)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    QColor gold(220, 180, 40, 245);
    QColor shine(255, 240, 120, 230);
    QColor dark(140, 100, 20, 240);

    p.fillRect(w / 4, h / 4, w / 2, h / 2, gold);
    p.fillRect(w / 3, h / 6, w / 3, h * 2 / 3, gold);
    p.fillRect(2, h / 3, w / 4 - 2, h / 3, gold);
    p.fillRect(w - w / 4, h / 3, w / 4 - 2, h / 3, gold);
    p.fillRect(w / 2 - 1, h / 4, 2, h / 2, dark);
    p.fillRect(w / 3, h / 2 - 1, w / 3, 2, shine);
    p.fillRect(w / 2 - 4, 2, 8, 6, gold);
    p.fillRect(w / 2 - 2, 0, 2, 4, dark);
    p.fillRect(w / 2 + 1, 0, 2, 4, dark);

    p.end();
    return pix;
}

static QPixmap createAltarSprite(int w = 80, int h = 60, bool active = false)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    QColor stone(100, 85, 60, 245);
    QColor dark(65, 50, 35, 240);
    QColor gold = active ? QColor(255, 210, 60, 230) : QColor(120, 100, 60, 200);
    QColor glow = active ? QColor(255, 240, 120, 180) : QColor(80, 70, 50, 150);

    p.fillRect(4, h - 14, w - 8, 14, dark);
    p.fillRect(0, h - 18, w, 6, stone);
    p.fillRect(10, h / 3, w - 20, h - h / 3 - 18, stone);
    p.fillRect(6, h / 3 - 4, w - 12, 8, stone);
    p.fillRect(14, h / 2, w - 28, 3, gold);
    p.fillRect(w / 2 - 8, h / 3, 16, h / 3, gold);
    p.fillRect(w / 2 - 6, h / 2 + 8, 12, 8, glow);
    p.fillRect(w / 2 - 3, h / 2 + 6, 6, 4, gold);
    p.fillRect(12, h / 3 + 2, 6, 6, gold);
    p.fillRect(w - 18, h / 3 + 2, 6, 6, gold);

    if (active) {
        p.setBrush(QColor(255, 230, 100, 100));
        p.setPen(Qt::NoPen);
        p.drawEllipse(w / 4, 0, w / 2, h / 3);
    }
    p.end();
    return pix;
}

// ── Interactable base ─────────────────────────────────────────────────────────

bool Interactable::isNear(QPointF playerPos, float radius) const
{
    return qdist(playerPos, m_center) < radius;
}

// ── ClueFragmentObj ───────────────────────────────────────────────────────────

ClueFragmentObj::ClueFragmentObj(qreal x, qreal y, QGraphicsScene *scene)
{
    m_glow = scene->addRect(x - 6, y - 6, 40, 40,
                             Qt::NoPen, QBrush(QColor(255, 210, 80, 40)));
    m_glow->setZValue(4);

    m_sprite = new QGraphicsPixmapItem(createClueSprite());
    m_sprite->setPos(x, y);
    m_sprite->setZValue(5);
    scene->addItem(m_sprite);

    m_center = m_sprite->sceneBoundingRect().center();
}

ClueFragmentObj::~ClueFragmentObj()
{
    if (m_sprite && m_sprite->scene()) { m_sprite->scene()->removeItem(m_sprite); delete m_sprite; }
    if (m_glow   && m_glow->scene())   { m_glow->scene()->removeItem(m_glow);     delete m_glow;   }
}

void ClueFragmentObj::update()
{
    if (m_done || !m_glow) return;
    m_glowPhase += 0.08f;
    int alpha = int(40 + 30 * qSin(m_glowPhase));
    m_glow->setBrush(QBrush(QColor(255, 210, 80, alpha)));
}

InteractResult ClueFragmentObj::interact()
{
    if (m_done) return {};
    m_done = true;
    if (m_sprite) m_sprite->setVisible(false);
    if (m_glow)   m_glow->setVisible(false);
    return { "", 0, true, false, false, false };
}

bool ClueFragmentObj::isNear(QPointF pos, float radius) const
{
    if (m_done || !m_sprite) return false;
    return qdist(pos, m_sprite->sceneBoundingRect().center()) < radius;
}

// ── StatueObj ─────────────────────────────────────────────────────────────────

StatueObj::StatueObj(qreal x, qreal y, const QString &lore, QGraphicsScene *scene)
    : m_lore(lore)
{
    m_sprite = new QGraphicsPixmapItem(createStatueSprite());
    m_sprite->setPos(x, y);
    m_sprite->setZValue(3);
    scene->addItem(m_sprite);

    m_hitbox = scene->addRect(x, y, 40, 80, Qt::NoPen, Qt::NoBrush);
    m_hitbox->setZValue(1);

    m_center = m_hitbox->sceneBoundingRect().center();
}

StatueObj::~StatueObj()
{
    if (m_sprite && m_sprite->scene()) { m_sprite->scene()->removeItem(m_sprite); delete m_sprite; }
    if (m_hitbox && m_hitbox->scene()) { m_hitbox->scene()->removeItem(m_hitbox); delete m_hitbox; }
}

InteractResult StatueObj::interact()
{
    return { m_lore };
}

// ── TabletObj ─────────────────────────────────────────────────────────────────

TabletObj::TabletObj(qreal x, qreal y, const QString &hint, QGraphicsScene *scene)
    : m_hint(hint)
{
    m_sprite = new QGraphicsPixmapItem(createTabletSprite());
    m_sprite->setPos(x, y);
    m_sprite->setZValue(3);
    scene->addItem(m_sprite);

    m_hitbox = scene->addRect(x, y, 50, 40, Qt::NoPen, Qt::NoBrush);
    m_hitbox->setZValue(1);

    m_center = m_hitbox->sceneBoundingRect().center();
}

TabletObj::~TabletObj()
{
    if (m_sprite && m_sprite->scene()) { m_sprite->scene()->removeItem(m_sprite); delete m_sprite; }
    if (m_hitbox && m_hitbox->scene()) { m_hitbox->scene()->removeItem(m_hitbox); delete m_hitbox; }
}

InteractResult TabletObj::interact()
{
    return { m_hint };
}

// ── TorchObj ──────────────────────────────────────────────────────────────────

TorchObj::TorchObj(qreal x, qreal y, QGraphicsScene *scene,
                   QList<QGraphicsItem*> reveals)
    : m_reveals(reveals)
{
    m_sprite = new QGraphicsPixmapItem(createTorchSprite(20, 45, false));
    m_sprite->setPos(x, y);
    m_sprite->setZValue(4);
    scene->addItem(m_sprite);

    m_hitbox = scene->addRect(x - 5, y, 30, 50, Qt::NoPen, Qt::NoBrush);
    m_hitbox->setZValue(1);

    m_center = m_hitbox->sceneBoundingRect().center();
}

TorchObj::~TorchObj()
{
    if (m_sprite && m_sprite->scene()) { m_sprite->scene()->removeItem(m_sprite); delete m_sprite; }
    if (m_hitbox && m_hitbox->scene()) { m_hitbox->scene()->removeItem(m_hitbox); delete m_hitbox; }
}

InteractResult TorchObj::interact()
{
    m_activated = !m_activated;
    if (m_sprite) m_sprite->setPixmap(createTorchSprite(20, 45, m_activated));

    if (m_activated) {
        for (auto *item : m_reveals) item->setVisible(true);
        return { "The torch ignites! Something stirs in the shadows...",
                 0, false, false, false, true };
    } else {
        for (auto *item : m_reveals) item->setVisible(false);
        return { "The torch dims..." };
    }
}

// ── RelicObj ──────────────────────────────────────────────────────────────────

RelicObj::RelicObj(qreal x, qreal y, bool startVisible, QGraphicsScene *scene)
    : m_visible(startVisible)
{
    m_sprite = new QGraphicsPixmapItem(createRelicSprite());
    m_sprite->setPos(x, y);
    m_sprite->setZValue(5);
    m_sprite->setVisible(startVisible);
    scene->addItem(m_sprite);
}

RelicObj::~RelicObj()
{
    if (m_sprite && m_sprite->scene()) { m_sprite->scene()->removeItem(m_sprite); delete m_sprite; }
}

InteractResult RelicObj::interact()
{
    if (m_done || !m_visible) return {};
    m_done = true;
    if (m_sprite) m_sprite->setVisible(false);
    return { "", 50, false, true, false, false };
}

bool RelicObj::isNear(QPointF pos, float radius) const
{
    if (!m_visible || m_done || !m_sprite) return false;
    return qdist(pos, m_sprite->sceneBoundingRect().center()) < radius;
}

void RelicObj::reveal()
{
    if (!m_visible && m_sprite) {
        m_visible = true;
        m_sprite->setVisible(true);
    }
}

// ── AltarObj ──────────────────────────────────────────────────────────────────

AltarObj::AltarObj(qreal x, qreal y, QGraphicsScene *scene)
{
    m_sprite = new QGraphicsPixmapItem(createAltarSprite(80, 60, false));
    m_sprite->setPos(x, y);
    m_sprite->setZValue(3);
    scene->addItem(m_sprite);

    m_hitbox = scene->addRect(x, y, 80, 60, Qt::NoPen, Qt::NoBrush);
    m_hitbox->setZValue(1);

    m_glowEffect = scene->addRect(x - 10, y - 20, 100, 100,
                                   Qt::NoPen, QBrush(QColor(255, 230, 100, 0)));
    m_glowEffect->setZValue(2);

    m_hint = scene->addText("Press E");
    m_hint->setDefaultTextColor(QColor(255, 255, 120));
    QFont hf; hf.setPixelSize(13); hf.setBold(true);
    m_hint->setFont(hf);
    m_hint->setPos(x + 10, y - 22);
    m_hint->setZValue(20);
    m_hint->setVisible(false);

    m_center = m_hitbox->sceneBoundingRect().center();
}

AltarObj::~AltarObj()
{
    if (m_sprite     && m_sprite->scene())     { m_sprite->scene()->removeItem(m_sprite);         delete m_sprite;     }
    if (m_hitbox     && m_hitbox->scene())     { m_hitbox->scene()->removeItem(m_hitbox);         delete m_hitbox;     }
    if (m_glowEffect && m_glowEffect->scene()) { m_glowEffect->scene()->removeItem(m_glowEffect); delete m_glowEffect; }
    if (m_hint       && m_hint->scene())       { m_hint->scene()->removeItem(m_hint);             delete m_hint;       }
}

void AltarObj::update()
{
    if (!m_active) return;

    // Pulse glow (identical math to original updateClueGlow())
    m_glowPhase += 0.06f;
    int alpha = int(60 + 40 * qSin(m_glowPhase));
    if (m_glowEffect)
        m_glowEffect->setBrush(QBrush(QColor(255, 230, 100, alpha)));

    // Show hint when player is within 120 px (original onGameLoop() logic)
    if (m_hint && m_hitbox) {
        float d = qdist(m_playerPos, m_hitbox->sceneBoundingRect().center());
        m_hint->setVisible(d < 120.f);
    }
}

InteractResult AltarObj::interact()
{
    if (!m_active) return {};
    return { "", 0, false, false, true, false };
}

bool AltarObj::isNear(QPointF pos, float radius) const
{
    if (!m_active || !m_hitbox) return false;
    return qdist(pos, m_hitbox->sceneBoundingRect().center()) < radius;
}

void AltarObj::activate(QGraphicsScene * /*scene*/)
{
    m_active = true;
    if (m_sprite)
        m_sprite->setPixmap(createAltarSprite(80, 60, true));
    if (m_glowEffect)
        m_glowEffect->setBrush(QBrush(QColor(255, 230, 100, 80)));
}
