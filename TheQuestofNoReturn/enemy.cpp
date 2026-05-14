#include "enemy.h"
#include <QPainter>
#include <QPixmap>

// ── Sprite creators (file-scope; identical pixel art to original) ─────────────

static QPixmap createMummySprite(int w, int h, bool dark = false)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    QColor bandage = dark ? QColor(112, 92, 72)  : QColor(215, 195, 155);
    QColor wrap    = dark ? QColor( 50, 36, 22)  : QColor(135, 105,  65);
    QColor eyes    = dark ? QColor(255,  50, 25)  : QColor( 45, 215,  75);

    auto strip = [&](int x, int y, int sw, int sh){ p.fillRect(x, y, sw, sh, wrap);    };
    auto block = [&](int x, int y, int bw, int bh){ p.fillRect(x, y, bw, bh, bandage); };

    int hH = h * 24 / 100, hW = w * 52 / 100, hX = (w - hW) / 2;
    block(hX, 0, hW, hH);
    strip(hX, hH / 3,     hW, qMax(1, h / 55));
    strip(hX, hH * 2 / 3, hW, qMax(1, h / 55));
    int eW = qMax(2, w * 7 / 100), eH = qMax(2, h * 4 / 100), eY = hH * 2 / 5;
    p.fillRect(hX + hW / 5,      eY, eW, eH, eyes);
    p.fillRect(hX + hW * 3 / 5,  eY, eW, eH, eyes);

    int bY = hH, bH = h * 46 / 100, bW = w * 68 / 100, bX = (w - bW) / 2;
    block(bX, bY, bW, bH);
    for (int i = 1; i <= 4; i++) strip(bX, bY + bH * i / 5, bW, qMax(1, h / 55));
    strip(w / 2 - 1, bY, 2, bH);

    int aW = qMax(4, w * 13 / 100), aH = bH * 55 / 100, aY = bY + bH / 10;
    block(bX - aW, aY, aW, aH);
    block(bX + bW, aY, aW, aH);
    strip(bX - aW, aY + aH / 2, aW, qMax(1, h / 65));
    strip(bX + bW, aY + aH / 2, aW, qMax(1, h / 65));

    int lY = bY + bH, lH = h - lY;
    int lW = w * 25 / 100, gap = qMax(2, w * 4 / 100);
    int llX = w / 2 - gap / 2 - lW, lrX = w / 2 + gap / 2;
    block(llX, lY, lW, lH);
    block(lrX, lY, lW, lH);
    for (int i = 1; i <= 2; i++) {
        strip(llX, lY + lH * i / 3, lW, qMax(1, h / 65));
        strip(lrX, lY + lH * i / 3, lW, qMax(1, h / 65));
    }
    int fe = qMax(1, w / 20);
    block(llX - fe, lY + lH * 4 / 5, lW + fe * 2, lH / 5 + 1);
    block(lrX - fe, lY + lH * 4 / 5, lW + fe * 2, lH / 5 + 1);

    p.end();
    return pix;
}

static QPixmap createSpecterSprite(int w, int h)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    QColor core(145, 75, 210, 190);
    QColor glow(185, 120, 255, 210);
    QColor fade(115, 50, 180, 135);
    QColor eyes(255, 210, 40, 245);

    int hW = w * 58 / 100, hH = h * 30 / 100, hX = (w - hW) / 2;
    p.fillRect(hX, 0, hW, hH, glow);
    p.fillRect(hX, hH / 3,     hW, qMax(1, h / 60), fade);
    p.fillRect(hX, hH * 2 / 3, hW, qMax(1, h / 60), fade);
    int eW = qMax(3, w / 9), eH = qMax(2, h / 14), eY = hH * 2 / 5;
    p.fillRect(hX + hW / 5,     eY, eW, eH, eyes);
    p.fillRect(hX + hW * 3 / 5, eY, eW, eH, eyes);

    int bY = hH * 4 / 5, bH = h * 42 / 100, bW = w * 70 / 100, bX = (w - bW) / 2;
    p.fillRect(bX, bY, bW, bH, core);
    for (int i = 1; i <= 3; i++)
        p.fillRect(bX, bY + bH * i / 4, bW, qMax(1, h / 60), fade);

    int tY = bY + bH, tH = h - tY, tW = bW / 4;
    for (int i = 0; i < 3; i++) {
        int tx = bX + (bW - tW) * i / 2;
        int th = (i % 2 == 0) ? tH : tH * 6 / 8;
        p.fillRect(tx, tY, tW - 2, th, fade);
    }

    p.end();
    return pix;
}

static QPixmap createScarabSprite(int w, int h)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    QColor shell(25, 88, 32, 240);
    QColor shine(68, 158, 68, 210);
    QColor lege (14, 48, 14, 235);
    QColor eyes (195, 162, 12, 245);

    int hdW = w * 42 / 100, hdH = h * 18 / 100, hdX = (w - hdW) / 2;
    p.fillRect(hdX, 0, hdW, hdH, shell);
    p.fillRect(hdX + hdW / 4 - 1,     0, 2, h / 8, lege);
    p.fillRect(hdX + hdW * 3 / 4 - 1, 0, 2, h / 8, lege);
    int eW = qMax(3, w / 11), eH = qMax(2, h / 15);
    p.fillRect(hdX + hdW / 5,      hdH / 3, eW, eH, eyes);
    p.fillRect(hdX + hdW * 3 / 5,  hdH / 3, eW, eH, eyes);

    int bY = hdH, bH = h * 72 / 100, bW = w * 76 / 100, bX = (w - bW) / 2;
    p.fillRect(bX, bY, bW, bH, shell);
    p.fillRect(bX + bW / 5, bY + bH / 7, bW / 3, bH / 4, shine);
    p.fillRect(w / 2 - 1, bY + bH / 8, 2, bH * 7 / 8, lege);
    p.fillRect(bX, bY + bH * 2 / 5, bW, qMax(1, h / 50), lege);

    int aY = bY + bH, aH = h - aY;
    if (aH > 0) p.fillRect(bX + bW / 8, aY, bW * 3 / 4, aH, shell);

    for (int i = 0; i < 3; i++) {
        int ly = bY + bH * (i * 2 + 1) / 7;
        int lLen = qMax(3, bX - 1);
        p.fillRect(0,           ly, lLen, qMax(2, h / 10), lege);
        p.fillRect(bX + bW + 1, ly, lLen, qMax(2, h / 10), lege);
    }

    p.end();
    return pix;
}

// ── Enemy base ────────────────────────────────────────────────────────────────

Enemy::Enemy(QGraphicsScene *scene,
             qreal x, qreal y, qreal w, qreal h,
             bool horiz, float mn, float mx, float spd, float startDir,
             float detectRadius, float chaseSpd)
    : m_horiz(horiz)
    , m_mn(mn), m_mx(mx), m_spd(spd), m_dir(startDir)
    , m_detectionRadius(detectRadius)
    , m_chaseSpeed(chaseSpd > 0 ? chaseSpd : spd * 1.5f)
    , m_chasing(false)
    , m_homeX(float(x) + float(w) / 2.f)
    , m_homeY(float(y) + float(h) / 2.f)
{
    m_item = new QGraphicsRectItem(0, 0, w, h);
    m_item->setPen(Qt::NoPen);
    m_item->setBrush(Qt::NoBrush);
    m_item->setPos(x, y);
    m_item->setZValue(5);
    scene->addItem(m_item);
    // m_sprite is set by each subclass constructor
}

Enemy::~Enemy()
{
    // m_item owns m_sprite as a child; deleting m_item removes both from the scene
    // The scene itself does NOT own m_item (we added it manually), so we delete here.
    if (m_item && m_item->scene())
        m_item->scene()->removeItem(m_item);
    delete m_item;
}

QRectF Enemy::boundingRect() const
{
    return m_item ? m_item->sceneBoundingRect() : QRectF{};
}

bool Enemy::collidesWith(QRectF other) const
{
    return m_item && m_item->sceneBoundingRect().intersects(other);
}

void Enemy::moveStep()
{
    if (!m_item) return;
    QPointF tp = m_item->pos();
    QPointF trapCenter = m_item->sceneBoundingRect().center();

    float distToPlayer = qSqrt(qPow(m_playerPos.x() - trapCenter.x(), 2) +
                               qPow(m_playerPos.y() - trapCenter.y(), 2));

    if (m_detectionRadius > 0 && distToPlayer < m_detectionRadius) {
        m_chasing = true;
        float cdx = float(m_playerPos.x() - trapCenter.x());
        float cdy = float(m_playerPos.y() - trapCenter.y());
        float clen = qSqrt(cdx * cdx + cdy * cdy);
        if (clen > 1.f) {
            float moveX = (cdx / clen) * m_chaseSpeed;
            float moveY = (cdy / clen) * m_chaseSpeed;
            m_item->setPos(tp.x() + moveX, tp.y() + moveY);
        }
    } else {
        m_chasing = false;
        if (m_horiz) {
            float nx = float(tp.x()) + m_spd * m_dir;
            if (nx <= m_mn) { nx = m_mn; m_dir = 1; }
            if (nx >= m_mx) { nx = m_mx; m_dir = -1; }
            m_item->setPos(nx, tp.y());
        } else {
            float ny = float(tp.y()) + m_spd * m_dir;
            if (ny <= m_mn) { ny = m_mn; m_dir = 1; }
            if (ny >= m_mx) { ny = m_mx; m_dir = -1; }
            m_item->setPos(tp.x(), ny);
        }
    }
}

// ── Mummy ─────────────────────────────────────────────────────────────────────

Mummy::Mummy(QGraphicsScene *scene,
             qreal x, qreal y, qreal w, qreal h,
             bool horiz, float mn, float mx, float spd, float startDir,
             float detectRadius, float chaseSpd)
    : Enemy(scene, x, y, w, h, horiz, mn, mx, spd, startDir, detectRadius, chaseSpd)
{
    m_sprite = new QGraphicsPixmapItem(createMummySprite(int(w), int(h), false), m_item);
    m_sprite->setPos(0, 0);
}

void Mummy::update()
{
    moveStep();
}

// ── Specter ───────────────────────────────────────────────────────────────────

Specter::Specter(QGraphicsScene *scene,
                 qreal x, qreal y, qreal w, qreal h,
                 bool horiz, float mn, float mx, float spd, float startDir,
                 float detectRadius, float chaseSpd)
    : Enemy(scene, x, y, w, h, horiz, mn, mx, spd, startDir, detectRadius, chaseSpd)
{
    m_sprite = new QGraphicsPixmapItem(createSpecterSprite(int(w), int(h)), m_item);
    m_sprite->setPos(0, 0);
}

void Specter::update()
{
    moveStep();
    // Opacity flicker every 18 ticks (identical to original logic)
    m_flickerTick++;
    if (m_flickerTick >= 18) {
        m_flickerTick = 0;
        if (m_sprite) {
            double op = (m_sprite->opacity() > 0.8) ? 0.60 : 0.95;
            m_sprite->setOpacity(op);
        }
    }
}

// ── Scarab ────────────────────────────────────────────────────────────────────

Scarab::Scarab(QGraphicsScene *scene,
               qreal x, qreal y, qreal w, qreal h,
               bool horiz, float mn, float mx, float spd, float startDir,
               float detectRadius, float chaseSpd)
    : Enemy(scene, x, y, w, h, horiz, mn, mx, spd, startDir, detectRadius, chaseSpd)
{
    m_sprite = new QGraphicsPixmapItem(createScarabSprite(int(w), int(h)), m_item);
    m_sprite->setPos(0, 0);
}

void Scarab::update()
{
    moveStep();
}

// ── DarkMummy ─────────────────────────────────────────────────────────────────

DarkMummy::DarkMummy(QGraphicsScene *scene,
                     qreal x, qreal y, qreal w, qreal h,
                     bool horiz, float mn, float mx, float spd, float startDir,
                     float detectRadius, float chaseSpd)
    : Enemy(scene, x, y, w, h, horiz, mn, mx, spd, startDir, detectRadius, chaseSpd)
{
    m_sprite = new QGraphicsPixmapItem(createMummySprite(int(w), int(h), true), m_item);
    m_sprite->setPos(0, 0);
}

void DarkMummy::update()
{
    moveStep();
}
