#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pausemenudialog.h"
#include "difficultydialog.h"
#include "howtoplaydialog.h"
#include "riddledialog.h"

#include <QKeyEvent>
#include <QPainter>
#include <QtMath>
#include <QRandomGenerator>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

static QPixmap createPlayerSprite(int frame = 0)
{
    QPixmap pix(44, 60);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    const QColor hat  ( 52,  28,   7);
    const QColor band (190, 145,  40);
    const QColor skin (235, 185, 110);
    const QColor eye  ( 25,  12,   0);
    const QColor shirt( 75, 100, 145);
    const QColor pants( 48,  32,  12);
    const QColor boot ( 35,  18,   4);

    // Cowboy hat dome
    p.fillRect(11,  2, 22, 11, hat);
    // Hat brim (wide)
    p.fillRect( 2, 10, 40,  5, hat);
    // Hat band
    p.fillRect(11, 10, 22,  2, band);

    // Face
    p.fillRect(14, 16, 17, 14, skin);
    // Eyes
    p.fillRect(17, 20,  2,  2, eye);
    p.fillRect(25, 20,  2,  2, eye);
    // Mouth
    p.fillRect(18, 27,  7,  2, QColor(170, 70, 50));

    // Neck
    p.fillRect(18, 30,  7,  4, skin);

    // Torso
    p.fillRect(11, 34, 22, 13, shirt);
    // Left arm
    p.fillRect( 4, 34,  8, 11, shirt);
    // Right arm
    p.fillRect(32, 34,  8, 11, shirt);
    // Hands
    p.fillRect( 4, 44,  8,  4, skin);
    p.fillRect(32, 44,  8,  4, skin);

    // Belt
    p.fillRect(11, 46, 22,  2, QColor(50, 28, 6));
    // Buckle
    p.fillRect(19, 46,  6,  2, band);

    // Legs and boots: frame 0 = neutral, frame 1 = stride (legs spread)
    int lo = (frame == 0) ? 0 : 2;   // outward offset per leg
    p.fillRect(12 - lo, 49,  9, 10, pants);
    p.fillRect(23 + lo, 49,  9, 10, pants);
    p.fillRect(10 - lo, 56, 11,  4, boot);
    p.fillRect(22 + lo, 56, 11,  4, boot);

    p.end();
    return pix;
}

// ─── Mummy enemy sprite (pass dark=true for cursed black-bandage variant) ───
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

    // Head (top 24 %)
    int hH = h * 24 / 100, hW = w * 52 / 100, hX = (w - hW) / 2;
    block(hX, 0, hW, hH);
    strip(hX, hH / 3,     hW, qMax(1, h / 55));
    strip(hX, hH * 2 / 3, hW, qMax(1, h / 55));
    // Eyes
    int eW = qMax(2, w * 7 / 100), eH = qMax(2, h * 4 / 100), eY = hH * 2 / 5;
    p.fillRect(hX + hW / 5,      eY, eW, eH, eyes);
    p.fillRect(hX + hW * 3 / 5,  eY, eW, eH, eyes);

    // Body (24 – 70 %)
    int bY = hH, bH = h * 46 / 100, bW = w * 68 / 100, bX = (w - bW) / 2;
    block(bX, bY, bW, bH);
    for (int i = 1; i <= 4; i++) strip(bX, bY + bH * i / 5, bW, qMax(1, h / 55));
    strip(w / 2 - 1, bY, 2, bH);   // centre cross-wrap

    // Arms
    int aW = qMax(4, w * 13 / 100), aH = bH * 55 / 100, aY = bY + bH / 10;
    block(bX - aW, aY, aW, aH);
    block(bX + bW, aY, aW, aH);
    strip(bX - aW, aY + aH / 2, aW, qMax(1, h / 65));
    strip(bX + bW, aY + aH / 2, aW, qMax(1, h / 65));

    // Legs (70 – 100 %)
    int lY = bY + bH, lH = h - lY;
    int lW = w * 25 / 100, gap = qMax(2, w * 4 / 100);
    int llX = w / 2 - gap / 2 - lW, lrX = w / 2 + gap / 2;
    block(llX, lY, lW, lH);
    block(lrX, lY, lW, lH);
    for (int i = 1; i <= 2; i++) {
        strip(llX, lY + lH * i / 3, lW, qMax(1, h / 65));
        strip(lrX, lY + lH * i / 3, lW, qMax(1, h / 65));
    }
    // Feet (slightly wider)
    int fe = qMax(1, w / 20);
    block(llX - fe, lY + lH * 4 / 5, lW + fe * 2, lH / 5 + 1);
    block(lrX - fe, lY + lH * 4 / 5, lW + fe * 2, lH / 5 + 1);

    p.end();
    return pix;
}

// ─── Specter / ghost-mummy enemy sprite (Room 2 – Hall of Echoes) ────────────
static QPixmap createSpecterSprite(int w, int h)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    QColor core(145, 75, 210, 190);
    QColor glow(185, 120, 255, 210);
    QColor fade(115, 50, 180, 135);
    QColor eyes(255, 210, 40, 245);

    // Head (top 30 %)
    int hW = w * 58 / 100, hH = h * 30 / 100, hX = (w - hW) / 2;
    p.fillRect(hX, 0, hW, hH, glow);
    // Bandage wraps across the face
    p.fillRect(hX, hH / 3,     hW, qMax(1, h / 60), fade);
    p.fillRect(hX, hH * 2 / 3, hW, qMax(1, h / 60), fade);
    // Eyes
    int eW = qMax(3, w / 9), eH = qMax(2, h / 14), eY = hH * 2 / 5;
    p.fillRect(hX + hW / 5,     eY, eW, eH, eyes);
    p.fillRect(hX + hW * 3 / 5, eY, eW, eH, eyes);

    // Body (25 – 68 %)
    int bY = hH * 4 / 5, bH = h * 42 / 100, bW = w * 70 / 100, bX = (w - bW) / 2;
    p.fillRect(bX, bY, bW, bH, core);
    for (int i = 1; i <= 3; i++)
        p.fillRect(bX, bY + bH * i / 4, bW, qMax(1, h / 60), fade);

    // Three wispy tendrils at bottom
    int tY = bY + bH, tH = h - tY, tW = bW / 4;
    for (int i = 0; i < 3; i++) {
        int tx = bX + (bW - tW) * i / 2;
        int th = (i % 2 == 0) ? tH : tH * 6 / 8;
        p.fillRect(tx, tY, tW - 2, th, fade);
    }

    p.end();
    return pix;
}

// ─── Khonshu boss sprite (Room 5 – Khonshu's Chamber) ───────────────────────
static QPixmap createKhonshuSprite(int w = 120, int h = 200)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    const QColor robeCol (220, 220, 240);
    const QColor wrapCol (140, 140, 185);
    const QColor headCol ( 50,  30,  10);
    const QColor crownCol(215, 185,  50);
    const QColor eyeCol  (255, 215,  50);
    const QColor collarBl( 28,  52, 148);
    const QColor jewelCol(195, 158,  38);
    const QColor staffCol(190, 155,  35);
    const QColor beakCol (160,  98,  18);
    const QColor bgCut   ( 8,   4,  28, 255);

    // ── Crescent moon crown ──────────────────────────────
    p.setBrush(crownCol); p.setPen(Qt::NoPen);
    p.drawEllipse(w/2 - 18, 0, 36, 26);
    p.setBrush(bgCut);
    p.drawEllipse(w/2 - 6, 0, 36, 26);

    // Crown disc
    p.setBrush(crownCol); p.setPen(QPen(QColor(255,230,100), 1));
    p.drawEllipse(w/2 - 11, 20, 22, 14);

    // ── Falcon head ──────────────────────────────────────
    int hdY = 30, hdW = 50, hdH = 38, hdX = (w - hdW) / 2;
    p.setBrush(headCol); p.setPen(Qt::NoPen);
    p.fillRect(hdX, hdY, hdW, hdH, headCol);
    p.fillRect(hdX + 4, hdY - 4, hdW - 8, 6, headCol);  // top rounding
    // Eye
    p.fillRect(hdX + hdW * 2/3, hdY + hdH/3, 7, 5, eyeCol);
    // Beak
    p.setBrush(beakCol);
    QPolygonF beak;
    beak << QPointF(hdX + hdW,      hdY + hdH * 0.40)
         << QPointF(hdX + hdW + 14, hdY + hdH * 0.52)
         << QPointF(hdX + hdW,      hdY + hdH * 0.68);
    p.drawPolygon(beak);

    // ── Broad collar ─────────────────────────────────────
    int colY = hdY + hdH, colH = 18, colW = 62, colX = (w - colW) / 2;
    p.fillRect(colX, colY, colW, colH, collarBl);
    for (int i = 0; i < 4; i++)
        p.fillRect(colX, colY + i * 4 + 1, colW, 2, jewelCol);

    // ── Robe body ─────────────────────────────────────────
    int bdY = colY + colH, bdW = 58, bdH = 94, bdX = (w - bdW) / 2;
    p.fillRect(bdX, bdY, bdW, bdH, robeCol);
    for (int i = 1; i <= 7; i++)
        p.fillRect(bdX, bdY + bdH * i / 8, bdW, 2, wrapCol);
    p.fillRect(w/2 - 1, bdY, 2, bdH, wrapCol);
    p.fillRect(w/2 - 8, bdY + 4, 16, 11, jewelCol);  // chest jewel

    // ── Arms ─────────────────────────────────────────────
    // Left arm (down)
    p.fillRect(bdX - 13, bdY + 4, 13, 54, robeCol);
    p.fillRect(bdX - 13, bdY + 4,  13, 2, wrapCol);
    p.fillRect(bdX - 13, bdY + 28, 13, 2, wrapCol);
    // Right arm (raised, holding staff)
    p.fillRect(bdX + bdW, bdY + 4, 13, 38, robeCol);
    p.fillRect(bdX + bdW, bdY + 4,  13, 2, wrapCol);
    p.fillRect(bdX + bdW, bdY + 22, 13, 2, wrapCol);

    // ── Staff ────────────────────────────────────────────
    int stX = bdX + bdW + 13 + 3;
    p.fillRect(stX - 2, 2, 4, h - 2, staffCol);
    // Staff crescent top
    p.setBrush(staffCol); p.setPen(Qt::NoPen);
    p.drawEllipse(stX - 11, 2, 22, 18);
    p.setBrush(bgCut);
    p.drawEllipse(stX - 3, 2, 22, 18);

    // ── Legs / base ──────────────────────────────────────
    int lgY = bdY + bdH, lgH = h - lgY;
    if (lgH > 0) {
        p.fillRect(bdX, lgY, bdW, lgH, robeCol);
        for (int i = 1; i < 4; i++)
            p.fillRect(bdX, lgY + i * lgH / 3, bdW, 2, wrapCol);
    }

    p.end();
    return pix;
}

// ─── Moon crescent projectile ─────────────────────────────────────────────────
static QPixmap createCrescentSprite(int w, int h)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    // Outer golden disc
    p.setBrush(QColor(220, 192, 60));
    p.setPen(QPen(QColor(255, 232, 110), 1));
    p.drawEllipse(1, 1, w - 2, h - 2);

    // Inner cut-out shifted right → crescent shape
    p.setBrush(QColor(8, 4, 28, 255));
    p.setPen(Qt::NoPen);
    p.drawEllipse(w / 3, 1, w - 2, h - 2);

    p.end();
    return pix;
}

// ─── Scarab beetle enemy sprite (Room 4 – Ankh Sanctuary) ────────────────────
static QPixmap createScarabSprite(int w, int h)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    QColor shell(25, 88, 32, 240);
    QColor shine(68, 158, 68, 210);
    QColor lege (14, 48, 14, 235);
    QColor eyes (195, 162, 12, 245);

    // Head (top 18 %)
    int hdW = w * 42 / 100, hdH = h * 18 / 100, hdX = (w - hdW) / 2;
    p.fillRect(hdX, 0, hdW, hdH, shell);
    // Antennae
    p.fillRect(hdX + hdW / 4 - 1,     0, 2, h / 8, lege);
    p.fillRect(hdX + hdW * 3 / 4 - 1, 0, 2, h / 8, lege);
    // Eyes
    int eW = qMax(3, w / 11), eH = qMax(2, h / 15);
    p.fillRect(hdX + hdW / 5,      hdH / 3, eW, eH, eyes);
    p.fillRect(hdX + hdW * 3 / 5,  hdH / 3, eW, eH, eyes);

    // Shell body (18 – 90 %)
    int bY = hdH, bH = h * 72 / 100, bW = w * 76 / 100, bX = (w - bW) / 2;
    p.fillRect(bX, bY, bW, bH, shell);
    p.fillRect(bX + bW / 5, bY + bH / 7, bW / 3, bH / 4, shine);   // highlight
    p.fillRect(w / 2 - 1, bY + bH / 8, 2, bH * 7 / 8, lege);        // wing split
    p.fillRect(bX, bY + bH * 2 / 5, bW, qMax(1, h / 50), lege);     // cross-ridge

    // Abdomen nub
    int aY = bY + bH, aH = h - aY;
    if (aH > 0) p.fillRect(bX + bW / 8, aY, bW * 3 / 4, aH, shell);

    // Legs – 3 per side
    for (int i = 0; i < 3; i++) {
        int ly = bY + bH * (i * 2 + 1) / 7;
        int lLen = qMax(3, bX - 1);
        p.fillRect(0,          ly, lLen,        qMax(2, h / 10), lege);
        p.fillRect(bX + bW + 1, ly, lLen,       qMax(2, h / 10), lege);
    }

    p.end();
    return pix;
}

// ─── Clue Fragment sprite (glowing golden shard) ─────────────────────────────
static QPixmap createClueSprite(int w = 28, int h = 28)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    // Outer glow
    p.setBrush(QColor(255, 210, 80, 60));
    p.setPen(Qt::NoPen);
    p.drawEllipse(2, 2, w - 4, h - 4);

    // Inner crystal shape
    QPolygonF crystal;
    crystal << QPointF(w / 2, 4)
            << QPointF(w - 6, h / 2)
            << QPointF(w / 2, h - 4)
            << QPointF(6, h / 2);
    p.setBrush(QColor(255, 200, 60, 230));
    p.setPen(QPen(QColor(255, 240, 150), 2));
    p.drawPolygon(crystal);

    // Highlight
    p.fillRect(w / 2 - 2, h / 3, 4, 4, QColor(255, 255, 200, 200));

    p.end();
    return pix;
}

// ─── Statue sprite (Egyptian figure) ─────────────────────────────────────────
static QPixmap createStatueSprite(int w = 40, int h = 80)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    QColor stone(120, 100, 70, 240);
    QColor dark(80, 65, 45, 230);
    QColor gold(200, 165, 50, 220);

    // Base pedestal
    p.fillRect(2, h - 12, w - 4, 12, dark);
    p.fillRect(4, h - 14, w - 8, 4, stone);

    // Body
    int bodyH = h - 20;
    p.fillRect(w / 4, 20, w / 2, bodyH - 6, stone);

    // Head (pharaoh headdress shape)
    p.fillRect(w / 4 - 4, 6, w / 2 + 8, 18, stone);
    p.fillRect(w / 4, 0, w / 2, 10, gold);

    // Face details
    p.fillRect(w / 2 - 4, 12, 3, 2, dark);
    p.fillRect(w / 2 + 2, 12, 3, 2, dark);
    p.fillRect(w / 2 - 2, 18, 5, 2, dark);

    // Arms crossed
    p.fillRect(w / 4 - 2, 28, w / 2 + 4, 6, stone);
    p.fillRect(w / 4, 32, 6, 20, stone);
    p.fillRect(w - w / 4 - 6, 32, 6, 20, stone);

    // Decorative bands
    p.fillRect(w / 4, 26, w / 2, 2, gold);
    p.fillRect(w / 4, 45, w / 2, 2, gold);

    p.end();
    return pix;
}

// ─── Tablet sprite (stone tablet with hieroglyphs) ───────────────────────────
static QPixmap createTabletSprite(int w = 50, int h = 40)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    QColor stone(160, 140, 100, 240);
    QColor dark(100, 85, 55, 230);
    QColor glyph(60, 45, 25, 200);

    // Main tablet
    p.fillRect(2, 4, w - 4, h - 6, stone);
    p.fillRect(0, 2, w, 4, dark);
    p.fillRect(0, h - 4, w, 4, dark);

    // Hieroglyphs (simple shapes)
    int glyphY = 12;
    for (int row = 0; row < 2; row++) {
        for (int col = 0; col < 4; col++) {
            int gx = 8 + col * 10;
            int gy = glyphY + row * 12;
            // Vary shapes
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

// ─── Torch sprite (wall torch) ───────────────────────────────────────────────
static QPixmap createTorchSprite(int w = 20, int h = 45, bool lit = false)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    QColor handle(90, 60, 30, 245);
    QColor metal(140, 120, 80, 240);

    // Handle
    p.fillRect(w / 2 - 3, h / 3, 6, h * 2 / 3, handle);

    // Bracket
    p.fillRect(w / 2 - 6, h / 3, 12, 4, metal);
    p.fillRect(w / 2 - 8, h / 3 - 2, 4, 8, metal);

    // Bowl
    p.fillRect(w / 2 - 7, 8, 14, 10, metal);

    if (lit) {
        // Flame
        p.setBrush(QColor(255, 180, 40, 200));
        p.setPen(Qt::NoPen);
        QPolygonF flame;
        flame << QPointF(w / 2, 0)
              << QPointF(w / 2 + 6, 12)
              << QPointF(w / 2, 8)
              << QPointF(w / 2 - 6, 12);
        p.drawPolygon(flame);
        // Inner flame
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

// ─── Relic sprite (golden scarab collectible) ────────────────────────────────
static QPixmap createRelicSprite(int w = 32, int h = 24)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    QColor gold(220, 180, 40, 245);
    QColor shine(255, 240, 120, 230);
    QColor dark(140, 100, 20, 240);

    // Body
    p.fillRect(w / 4, h / 4, w / 2, h / 2, gold);
    p.fillRect(w / 3, h / 6, w / 3, h * 2 / 3, gold);

    // Wings
    p.fillRect(2, h / 3, w / 4 - 2, h / 3, gold);
    p.fillRect(w - w / 4, h / 3, w / 4 - 2, h / 3, gold);

    // Details
    p.fillRect(w / 2 - 1, h / 4, 2, h / 2, dark);
    p.fillRect(w / 3, h / 2 - 1, w / 3, 2, shine);

    // Head
    p.fillRect(w / 2 - 4, 2, 8, 6, gold);
    p.fillRect(w / 2 - 2, 0, 2, 4, dark);
    p.fillRect(w / 2 + 1, 0, 2, 4, dark);

    p.end();
    return pix;
}

// ─── Altar sprite (central altar, dormant or active) ─────────────────────────
static QPixmap createAltarSprite(int w = 80, int h = 60, bool active = false)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    QColor stone(100, 85, 60, 245);
    QColor dark(65, 50, 35, 240);
    QColor gold = active ? QColor(255, 210, 60, 230) : QColor(120, 100, 60, 200);
    QColor glow = active ? QColor(255, 240, 120, 180) : QColor(80, 70, 50, 150);

    // Base platform
    p.fillRect(4, h - 14, w - 8, 14, dark);
    p.fillRect(0, h - 18, w, 6, stone);

    // Main altar body
    p.fillRect(10, h / 3, w - 20, h - h / 3 - 18, stone);

    // Top surface
    p.fillRect(6, h / 3 - 4, w - 12, 8, stone);

    // Decorative carvings
    p.fillRect(14, h / 2, w - 28, 3, gold);
    p.fillRect(w / 2 - 8, h / 3, 16, h / 3, gold);

    // Eye of Horus symbol on front
    p.fillRect(w / 2 - 6, h / 2 + 8, 12, 8, glow);
    p.fillRect(w / 2 - 3, h / 2 + 6, 6, 4, gold);

    // Corner decorations
    p.fillRect(12, h / 3 + 2, 6, 6, gold);
    p.fillRect(w - 18, h / 3 + 2, 6, 6, gold);

    if (active) {
        // Glowing aura on top
        p.setBrush(QColor(255, 230, 100, 100));
        p.setPen(Qt::NoPen);
        p.drawEllipse(w / 4, 0, w / 2, h / 3);
    }

    p.end();
    return pix;
}

// ═══════════════════════════════════════════════════════
// ROOM NARRATIVE
// ═══════════════════════════════════════════════════════

struct RoomInfo {
    QString name;
    QString enterText;   // shown in eventLabel when room loads
    QString solvedText;  // shown when riddle is answered correctly
    bool    survivorHeals;
    int     baseScore;
    int     bonusScore;
};

static const RoomInfo ROOMS[7] = {
    { "Entrance Chamber",
      "Explore the chamber. Collect 3 clue fragments to awaken the altar.",
      "The seal breaks — the passage is open!",
      false, 100, 60 },
    { "Guardian Hall",
      "A vigilant guardian patrols. Use cover, find clues, avoid detection.",
      "The colossus crumbles — you may pass!",
      false, 120, 60 },
    { "Hall of Echoes",
      "Spectres drift through illusions. Beware false doors. Seek the true altar.",
      "The echoes answer your wisdom — the path clears!",
      false, 130, 60 },
    { "Trial Room",
      "Deadly spikes rise in rhythm. Time your steps. Gather the clues.",
      "The stone tablet shatters — trial complete!",
      false, 150, 70 },
    { "Ankh Sanctuary",
      "Sacred scarabs guard hidden secrets. Light the torch to reveal the way.",
      "The Ankh glows — it has chosen you!",
      true, 140, 60 },
    { "Khonshu's Chamber",
      "The Moon God rises! Dodge the crescents — press R near Khonshu when he tires.",
      "Khonshu falls — the final gate lies ahead!",
      false, 300, 100 },
    { "Anubis Gate",
      "The floor crumbles beneath your feet. Move carefully. Find the final clues.",
      "Anubis bows. The gate opens — you have conquered the tomb!",
      false, 200, 80 }
};

// ═══════════════════════════════════════════════════════
// RIDDLES
// ═══════════════════════════════════════════════════════

static const RiddleDialog::Riddle RIDDLES[7] = {
    { "A seal on the passage demands:\n\n"
      "\"I speak without a mouth, hear without ears,\n"
      "have no body, yet come alive with wind. What am I?\"",
      {"A Shadow", "An Echo", "The Desert Wind", "A Torch Flame"}, 1,
      "Scholar Insight: Think of sound bouncing off these ancient stone walls..." },

    { "The stone guardian intones:\n\n"
      "\"The more you take, the more you leave behind. What am I?\"",
      {"Time", "Sand", "Footsteps", "Gold"}, 2,
      "Scholar Insight: Consider what remains on the ground with each step..." },

    { "Glowing runes whisper:\n\n"
      "\"I have cities, but no houses.\nI have mountains, but no trees.\n"
      "I have water, but no fish. What am I?\"",
      {"A Dream", "A Map", "A Painting", "A Stone Tablet"}, 1,
      "Scholar Insight: Explorers carry this to navigate the unknown..." },

    { "The stone tablet blazes:\n\n"
      "\"What has keys but no locks,\nspace but no room,\n"
      "and you can enter — but not go inside?\"",
      {"A House", "A Keyboard", "A Chest", "A Gateway"}, 1,
      "Scholar Insight: In the modern world, you type on this every day..." },

    { "The Ankh floats and asks:\n\n"
      "\"The Ankh is the symbol of eternal life.\n"
      "Which two basic shapes combine to form it?\"",
      {"A circle and a cross", "A triangle and a line",
       "A star and a crescent", "Two crossing lines"}, 0,
      "Scholar Insight: Trace the Ankh on this altar — find its geometric outline..." },

    // Room 5 — boss room, riddle never shown; placeholder required for array size
    { "", {"", "", "", ""}, 0, "" },

    { "Anubis speaks:\n\n"
      "\"I weigh the heart against the Feather of Ma'at.\n"
      "Which beast devours the hearts of the unworthy\n"
      "in the Hall of Two Truths?\"",
      {"The Sphinx", "Sobek", "Ammit", "Apep"}, 2,
      "Scholar Insight: It has the head of a crocodile, body of a lion,\n"
      "and hindquarters of a hippopotamus..." }
};

// ═══════════════════════════════════════════════════════
// ROOM-MAP SIDEBAR STYLES
// ═══════════════════════════════════════════════════════

static const QString S_NORMAL =
    "QLabel{background:rgba(15,10,5,180);color:#e2cfa0;"
    "border-left:3px solid #c8a03c;border-radius:6px;"
    "padding-left:8px;font-size:12px;font-family:\"Copperplate\";}";

static const QString S_ACTIVE =
    "QLabel{background:rgba(200,160,60,210);color:#0d0a05;"
    "border-left:4px solid #ffffc0;border-radius:6px;"
    "padding-left:8px;font-size:12px;font-family:\"Copperplate\";font-weight:bold;}";

static const QString S_DONE =
    "QLabel{background:rgba(0,80,20,160);color:#90ffb0;"
    "border-left:3px solid #00cc44;border-radius:6px;"
    "padding-left:8px;font-size:12px;font-family:\"Copperplate\";}";


// ═══════════════════════════════════════════════════════
// CONSTRUCTOR
// ═══════════════════════════════════════════════════════

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , selectedCharacter("Zara")
    , selectedTrait("Scholar")
    , playerName("")
    , selectedDifficulty(Difficulty::MEDIUM)
    , timeSeconds(0)
    , lives(0)
    , maxLives(0)
    , score(0)
    , currentRoom(0)
    , gameActive(false)
    , invincible(false)
    , invFrames(0)
    , riddleOpen(false)
    , animTick(0)
    , animFrame(0)
    , scene(nullptr)
    , playerSprite(nullptr)
    , doorItem(nullptr)
    , pressEHint(nullptr)
    , bossSprite(nullptr)
    , bossHPBar(nullptr)
    , bossState(BossState::INACTIVE)
    , bossHP(0)
    , bossShootCount(0)
    , bossShootTimer(0)
    , bossTiredTimer(0)
    , bossHitsThisPhase(0)
    , bossDyingTimer(0)
    , cluesFound(0)
    , requiredClues(3)
    , relicsCollected(0)
    , totalRelicsInGame(0)
    , currentMechanic(RoomMechanic::NONE)
    , realDoor(nullptr)
    , spikeTimer(0)
{
    altar = {nullptr, nullptr, nullptr, nullptr, false};
    ui->setupUi(this);

    // ── Timers ──────────────────────────────────────────
    countdownTimer = new QTimer(this);
    countdownTimer->setInterval(1000);
    connect(countdownTimer, &QTimer::timeout, this, &MainWindow::onCountdownTick);

    loopTimer = new QTimer(this);
    loopTimer->setInterval(16);   // ≈ 60 fps
    connect(loopTimer, &QTimer::timeout, this, &MainWindow::onGameLoop);

    // ── Room-map sidebar labels (index matches room order) ─
    mapLabels[0] = ui->label_7;   // Entrance Chamber
    mapLabels[1] = ui->label_10;  // Guardian Hall
    mapLabels[2] = ui->label_12;  // Hall of Echoes
    mapLabels[3] = ui->label_11;  // Trial Room
    mapLabels[4] = ui->label_8;   // Ankh Sanctuary
    mapLabels[5] = ui->label_9;   // Anubis Gate

    // ── Prevent QGraphicsView from stealing key focus ───
    ui->graphicsView->setFocusPolicy(Qt::NoFocus);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setInteractive(false);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);

    // ── Start page ──────────────────────────────────────
    ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);

    // ── Image scaling ───────────────────────────────────
    ui->portraitLabel->setScaledContents(true);
    ui->traitIconLabel->setScaledContents(true);
    ui->hudPortraitLabel->setScaledContents(true);
    ui->inventorySlot1->setScaledContents(true);
    ui->inventorySlot2->setScaledContents(true);
    ui->inventorySlot3->setScaledContents(true);
    ui->inventorySlot4->setScaledContents(true);

    // ── Character setup defaults ────────────────────────
    ui->portraitLabel->setPixmap(QPixmap(":/images/Zara.png"));
    ui->traitIconLabel->setPixmap(QPixmap(":/images/scholarIcon.png"));
    ui->descriptionLabel->setText("Fearless explorer driven by curiosity and courage.");
    ui->traitDescriptionLabel->setText("Gain intelligence bonuses and uncover hidden clues.");

    // ── HUD defaults (will be overwritten by startGame) ─
    ui->hudPortraitLabel->setPixmap(QPixmap(":/images/Zara.png"));
    ui->hudNameLabel->setText("ZARA");
    ui->hudTraitLabel->setText("SCHOLAR");
    ui->livesLabel->setText("♥ ♥ ♥");
    ui->timerLabel->setText("08:00");
    ui->scoreLabel->setText("🏆 0");
    ui->levelLabel->setText("1 / 6");
    ui->roomNameLabel->setText("Temple Entrance");
    ui->eventLabel->setText("A cold wind echoes through the chamber...");
    ui->controlsLabel->setText("WASD Move | E Interact | ESC Pause");

    // ── Inventory slots ─────────────────────────────────
    ui->inventorySlot1->setText(""); ui->inventorySlot1->setPixmap(QPixmap());
    ui->inventorySlot2->setText(""); ui->inventorySlot2->setPixmap(QPixmap());
    ui->inventorySlot3->setText(""); ui->inventorySlot3->setPixmap(QPixmap());
    ui->inventorySlot4->setText(""); ui->inventorySlot4->setPixmap(QPixmap());

    // ── Game-over / win backgrounds ─────────────────────
    ui->gameOverBackground->setPixmap(QPixmap(":/images/gameover_background.png"));
    ui->gameOverBackground->setScaledContents(true);
    ui->winBackgroundLabel->setPixmap(QPixmap(":/images/winBackground.png"));
    ui->winBackgroundLabel->setScaledContents(true);
    ui->ankhLabel->setPixmap(QPixmap(":/images/ankh.png"));
    ui->ankhLabel->setScaledContents(true);


    // ═══════════════════════════════════════════════════
    // MAIN MENU BUTTONS
    // ═══════════════════════════════════════════════════

    connect(ui->startButton, &QPushButton::clicked,
            this, [=]() {
                ui->stackedWidget->setCurrentWidget(ui->characterSetupScreen);
            });

    connect(ui->exitButton, &QPushButton::clicked,
            this, [=]() {
                close();
            });


    // ═══════════════════════════════════════════════════
    // CHARACTER SELECTION
    // ═══════════════════════════════════════════════════

    connect(ui->zaraButton, &QPushButton::clicked,
            this, [=]() {
                selectedCharacter = "Zara";
                ui->portraitLabel->setPixmap(QPixmap(":/images/Zara.png"));
                ui->descriptionLabel->setText("Fearless explorer driven by curiosity and courage.");
            });

    connect(ui->karimButton, &QPushButton::clicked,
            this, [=]() {
                selectedCharacter = "Karim";
                ui->portraitLabel->setPixmap(QPixmap(":/images/Karim.png"));
                ui->descriptionLabel->setText("Balanced adventurer skilled in combat and survival.");
            });

    connect(ui->nourButton, &QPushButton::clicked,
            this, [=]() {
                selectedCharacter = "Nour";
                ui->portraitLabel->setPixmap(QPixmap(":/images/NOOR.png"));
                ui->descriptionLabel->setText("Strategic thinker who uncovers hidden ancient secrets.");
            });

    connect(ui->ramiButton, &QPushButton::clicked,
            this, [=]() {
                selectedCharacter = "Rami";
                ui->portraitLabel->setPixmap(QPixmap(":/images/RAMI.png"));
                ui->descriptionLabel->setText("Fast and daring explorer who trusts instinct.");
            });


    // ═══════════════════════════════════════════════════
    // TRAIT SELECTION
    // ═══════════════════════════════════════════════════

    connect(ui->scholarButton, &QPushButton::clicked,
            this, [=]() {
                selectedTrait = "Scholar";
                ui->traitDescriptionLabel->setText("Gain intelligence bonuses and uncover hidden clues.");
                ui->traitIconLabel->setPixmap(QPixmap(":/images/scholarIcon.png"));
            });

    connect(ui->survivorButton, &QPushButton::clicked,
            this, [=]() {
                selectedTrait = "Survivor";
                ui->traitDescriptionLabel->setText("Higher endurance and resistance to danger.");
                ui->traitIconLabel->setPixmap(QPixmap(":/images/survivorIcon.png"));
            });

    connect(ui->speedsterButton, &QPushButton::clicked,
            this, [=]() {
                selectedTrait = "Speedster";
                ui->traitDescriptionLabel->setText("Move faster and escape traps more easily.");
                ui->traitIconLabel->setPixmap(QPixmap(":/images/speedsterIcon.png"));
            });


    // ═══════════════════════════════════════════════════
    // CONFIRM CHARACTER BUTTON
    // ═══════════════════════════════════════════════════

    connect(ui->confirmButton, &QPushButton::clicked,
            this, [=]() {

                playerName = ui->lineEdit->text();

                if (playerName.isEmpty()) {
                    ui->traitDescriptionLabel->setText("Enter your explorer name first.");
                    return;
                }

                // Update HUD with selected character
                ui->hudNameLabel->setText(playerName.toUpper());
                ui->hudTraitLabel->setText(selectedTrait.toUpper());

                if (selectedCharacter == "Zara")
                    ui->hudPortraitLabel->setPixmap(QPixmap(":/images/Zara.png"));
                else if (selectedCharacter == "Karim")
                    ui->hudPortraitLabel->setPixmap(QPixmap(":/images/Karim.png"));
                else if (selectedCharacter == "Nour")
                    ui->hudPortraitLabel->setPixmap(QPixmap(":/images/NOOR.png"));
                else if (selectedCharacter == "Rami")
                    ui->hudPortraitLabel->setPixmap(QPixmap(":/images/RAMI.png"));

                ui->stackedWidget->setCurrentWidget(ui->storyPage);
            });


    // ═══════════════════════════════════════════════════
    // ENTER TOMB BUTTON  — FIX: now calls startGame()
    // ═══════════════════════════════════════════════════

    connect(ui->enterTombButton, &QPushButton::clicked,
            this, [=]() {
                ui->stackedWidget->setCurrentWidget(ui->gamePage);
                startGame();   // ← was missing entirely before
            });


    // ═══════════════════════════════════════════════════
    // PAUSE MENU BUTTON
    // ═══════════════════════════════════════════════════

    connect(ui->pauseTestButton, &QPushButton::clicked,
            this, [=]() {

                if (!gameActive) return;
                loopTimer->stop();
                countdownTimer->stop();
                heldKeys.clear();

                PauseMenuDialog dialog(this);

                connect(&dialog, &PauseMenuDialog::resumeClicked,
                        this, [=]() {
                            if (gameActive) {
                                countdownTimer->start();
                                loopTimer->start();
                            }
                        });

                connect(&dialog, &PauseMenuDialog::restartClicked,
                        this, [=]() {
                            resetGame();
                            ui->stackedWidget->setCurrentWidget(ui->gamePage);
                            startGame();
                        });

                connect(&dialog, &PauseMenuDialog::mainMenuClicked,
                        this, [=]() {
                            resetGame();
                            ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);
                        });

                connect(&dialog, &PauseMenuDialog::exitClicked,
                        this, [=]() {
                            close();
                        });

                dialog.exec();

                // If dialog was closed without a signal (e.g. window X), resume
                if (gameActive && !countdownTimer->isActive()) {
                    countdownTimer->start();
                    loopTimer->start();
                }
            });


    // ═══════════════════════════════════════════════════
    // GAME OVER BUTTONS
    // ═══════════════════════════════════════════════════

    connect(ui->tryAgainButton, &QPushButton::clicked,
            this, [=]() {
                resetGame();
                ui->stackedWidget->setCurrentWidget(ui->gamePage);
                startGame();
            });

    connect(ui->returnMenuButton, &QPushButton::clicked,
            this, [=]() {
                resetGame();
                ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);
            });


    // ═══════════════════════════════════════════════════
    // WIN SCREEN BUTTONS
    // ═══════════════════════════════════════════════════

    connect(ui->playAgainButton, &QPushButton::clicked,
            this, [=]() {
                resetGame();
                ui->stackedWidget->setCurrentWidget(ui->gamePage);
                startGame();
            });

    connect(ui->leaveTempleButton, &QPushButton::clicked,
            this, [=]() {
                resetGame();
                ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);
            });


    // ═══════════════════════════════════════════════════
    // DIFFICULTY BUTTON  — FIX: now stores value, not just updates labels
    // ═══════════════════════════════════════════════════

    connect(ui->difficultyButton, &QPushButton::clicked,
            this, [=]() {

                DifficultyDialog dialog(this);

                connect(&dialog, &DifficultyDialog::difficultySelected,
                        this, [=](Difficulty difficulty) {

                            selectedDifficulty = difficulty;  // ← was never stored before

                            if (difficulty == Difficulty::EASY) {
                                ui->timerLabel->setText("12:00");
                                ui->livesLabel->setText("♥ ♥ ♥ ♥");
                            } else if (difficulty == Difficulty::MEDIUM) {
                                ui->timerLabel->setText("08:00");
                                ui->livesLabel->setText("♥ ♥ ♥");
                            } else if (difficulty == Difficulty::HARD) {
                                ui->timerLabel->setText("05:00");
                                ui->livesLabel->setText("♥ ♥");
                            }
                        });

                dialog.exec();
            });


    // ═══════════════════════════════════════════════════
    // HOW TO PLAY BUTTON
    // ═══════════════════════════════════════════════════

    connect(ui->howToPlayButton, &QPushButton::clicked,
            this, [=]() {
                HowToPlayDialog dialog(this);
                dialog.exec();
            });
}


// ═══════════════════════════════════════════════════════
// DESTRUCTOR
// ═══════════════════════════════════════════════════════

MainWindow::~MainWindow()
{
    delete ui;
}


// ═══════════════════════════════════════════════════════
// KEY EVENTS  — WASD to move, E to interact, ESC to pause
// ═══════════════════════════════════════════════════════

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    if (ui->stackedWidget->currentWidget() == ui->gamePage && gameActive) {
        switch (event->key()) {
        case Qt::Key_W: case Qt::Key_Up:    heldKeys.insert(Qt::Key_W); return;
        case Qt::Key_S: case Qt::Key_Down:  heldKeys.insert(Qt::Key_S); return;
        case Qt::Key_A: case Qt::Key_Left:  heldKeys.insert(Qt::Key_A); return;
        case Qt::Key_D: case Qt::Key_Right: heldKeys.insert(Qt::Key_D); return;
        case Qt::Key_E:   tryInteract();                                 return;
        case Qt::Key_R:   tryBossHit();                                  return;
        case Qt::Key_Escape: ui->pauseTestButton->click();               return;
        default: break;
        }
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;
    switch (event->key()) {
    case Qt::Key_W: case Qt::Key_Up:    heldKeys.remove(Qt::Key_W); break;
    case Qt::Key_S: case Qt::Key_Down:  heldKeys.remove(Qt::Key_S); break;
    case Qt::Key_A: case Qt::Key_Left:  heldKeys.remove(Qt::Key_A); break;
    case Qt::Key_D: case Qt::Key_Right: heldKeys.remove(Qt::Key_D); break;
    default: break;
    }
    QMainWindow::keyReleaseEvent(event);
}


// ═══════════════════════════════════════════════════════
// COUNTDOWN TICK  — fires every 1 second
// ═══════════════════════════════════════════════════════

void MainWindow::onCountdownTick()
{
    if (timeSeconds > 0) timeSeconds--;
    refreshTimer();
    if (timeSeconds == 0) {
        countdownTimer->stop();
        triggerGameOver("Time has run out. The tomb has claimed you.");
    }
}


// ═══════════════════════════════════════════════════════
// GAME LOOP  — fires every ~16 ms (≈60 fps)
// ═══════════════════════════════════════════════════════

void MainWindow::onGameLoop()
{
    if (!gameActive || !playerSprite) return;

    // ── Move player ────────────────────────────────────
    float spd = (selectedTrait == "Speedster") ? 7.f : 5.f;
    float dx = 0, dy = 0;
    if (heldKeys.contains(Qt::Key_W)) dy -= spd;
    if (heldKeys.contains(Qt::Key_S)) dy += spd;
    if (heldKeys.contains(Qt::Key_A)) dx -= spd;
    if (heldKeys.contains(Qt::Key_D)) dx += spd;
    if (dx && dy) { dx *= 0.707f; dy *= 0.707f; }

    QPointF op = playerSprite->pos();
    float nx = qBound(0.f, float(op.x()) + dx, 836.f);
    float ny = qBound(0.f, float(op.y()) + dy, 346.f);

    // Axis-separated obstacle collision so the player slides along walls
    if (!obstacles.isEmpty()) {
        playerSprite->setPos(nx, op.y());
        QRectF prx = playerSprite->sceneBoundingRect().adjusted(3, 3, -3, -3);
        for (auto *obs : obstacles)
            if (prx.intersects(obs->sceneBoundingRect())) { nx = op.x(); break; }

        playerSprite->setPos(nx, ny);
        QRectF pry = playerSprite->sceneBoundingRect().adjusted(3, 3, -3, -3);
        for (auto *obs : obstacles)
            if (pry.intersects(obs->sceneBoundingRect())) { ny = op.y(); break; }
    }
    playerSprite->setPos(nx, ny);

    // ── Walk animation ────────────────────────────────
    bool isMoving = (dx != 0.f || dy != 0.f);
    if (isMoving) {
        if (++animTick >= 8) {
            animTick = 0;
            animFrame ^= 1;
            playerSprite->setPixmap(createPlayerSprite(animFrame));
        }
    } else if (animFrame != 0) {
        animFrame = 0;
        animTick  = 0;
        playerSprite->setPixmap(createPlayerSprite(0));
    }

    // ── Move traps (with chase behavior) ───────────────
    QPointF playerCenter = playerSprite->sceneBoundingRect().center();
    for (auto &t : traps) {
        QPointF tp = t.item->pos();
        QPointF trapCenter = t.item->sceneBoundingRect().center();

        // Check if player is within detection radius
        float distToPlayer = qSqrt(qPow(playerCenter.x() - trapCenter.x(), 2) +
                                    qPow(playerCenter.y() - trapCenter.y(), 2));

        if (t.detectionRadius > 0 && distToPlayer < t.detectionRadius) {
            // Chase mode: move toward player
            t.chasing = true;
            float cdx = float(playerCenter.x() - trapCenter.x());
            float cdy = float(playerCenter.y() - trapCenter.y());
            float clen = qSqrt(cdx * cdx + cdy * cdy);
            if (clen > 1.f) {
                float moveX = (cdx / clen) * t.chaseSpeed;
                float moveY = (cdy / clen) * t.chaseSpeed;
                t.item->setPos(tp.x() + moveX, tp.y() + moveY);
            }
        } else {
            // Normal patrol mode
            t.chasing = false;
            if (t.horiz) {
                float nx = float(tp.x()) + t.spd * t.dir;
                if (nx <= t.mn) { nx = t.mn; t.dir = 1; }
                if (nx >= t.mx) { nx = t.mx; t.dir = -1; }
                t.item->setPos(nx, tp.y());
            } else {
                float ny = float(tp.y()) + t.spd * t.dir;
                if (ny <= t.mn) { ny = t.mn; t.dir = 1; }
                if (ny >= t.mx) { ny = t.mx; t.dir = -1; }
                t.item->setPos(tp.x(), ny);
            }
        }
    }

    // ── Specter flicker ───────────────────────────────
    if (animTick % 18 == 0) {
        for (auto &t : traps) {
            if (t.type == 1 && t.sprite) {
                double op = (t.sprite->opacity() > 0.8) ? 0.60 : 0.95;
                t.sprite->setOpacity(op);
            }
        }
    }

    // ── Invincibility flash ────────────────────────────
    if (invincible) {
        invFrames--;
        playerSprite->setOpacity((invFrames / 5) % 2 == 0 ? 1.0 : 0.25);
        if (invFrames <= 0) {
            invincible = false;
            playerSprite->setOpacity(1.0);
        }
    } else {
        // ── Trap collision ──────────────────────────────
        QRectF pr = playerSprite->sceneBoundingRect();
        for (auto &t : traps) {
            if (pr.intersects(t.item->sceneBoundingRect())) {
                onTrapHit();
                break;
            }
        }
    }

    // ── Clue fragment glow animation ──────────────────
    updateClueGlow();

    // ── Altar hint visibility ─────────────────────────
    if (altar.sprite && altar.hint && altar.hitbox) {
        QPointF pc = playerSprite->sceneBoundingRect().center();
        QPointF ac = altar.hitbox->sceneBoundingRect().center();
        float dist = qSqrt(qPow(pc.x() - ac.x(), 2) + qPow(pc.y() - ac.y(), 2));
        altar.hint->setVisible(altar.active && dist < 120.f);
    }

    // ── Room-specific mechanics ───────────────────────
    if (currentMechanic == RoomMechanic::SPIKE_TRAPS) {
        updateSpikeTraps();
    }
    if (currentMechanic == RoomMechanic::COLLAPSING_FLOOR) {
        updateCollapsingFloor();
    }
    if (currentMechanic == RoomMechanic::FAKE_DOORS) {
        checkFakeDoorCollision();
    }

    // ── Boss fight (room 5 only) ───────────────────────
    if (currentRoom == 5) {
        updateBoss();
        updateCrescents();
    }
}


// ═══════════════════════════════════════════════════════
// START GAME  — initialises everything; called from enterTombButton
// ═══════════════════════════════════════════════════════

void MainWindow::startGame()
{
    switch (selectedDifficulty) {
    case Difficulty::EASY: timeSeconds = 720; maxLives = 4; break;
    case Difficulty::HARD: timeSeconds = 300; maxLives = 2; break;
    default:               timeSeconds = 480; maxLives = 3; break;
    }

    lives      = maxLives;
    score      = 0;
    currentRoom = 0;
    gameActive  = true;
    invincible  = false;
    invFrames   = 0;
    riddleOpen  = false;
    heldKeys.clear();

    // Boss state reset
    bossState        = BossState::INACTIVE;
    bossHP           = 25;
    bossShootCount   = 0;
    bossShootTimer   = 0;
    bossTiredTimer   = 0;
    bossHitsThisPhase= 0;
    bossDyingTimer   = 0;

    // Exploration mechanics reset
    cluesFound       = 0;
    requiredClues    = 3;
    relicsCollected  = 0;
    totalRelicsInGame= 7;
    currentMechanic  = RoomMechanic::NONE;
    spikeTimer       = 0;

    // Sidebar: slot 5 now shows the boss room (7-room layout)
    mapLabels[5]->setText("Khonshu Boss");

    // Reset room-map sidebar
    for (int i = 0; i < 6; i++) mapLabels[i]->setStyleSheet(S_NORMAL);

    // Clear inventory
    ui->inventorySlot1->setPixmap(QPixmap()); ui->inventorySlot1->setText("");
    ui->inventorySlot2->setPixmap(QPixmap()); ui->inventorySlot2->setText("");
    ui->inventorySlot3->setPixmap(QPixmap()); ui->inventorySlot3->setText("");
    ui->inventorySlot4->setPixmap(QPixmap()); ui->inventorySlot4->setText("");

    refreshTimer();
    refreshLives();
    ui->scoreLabel->setText("🏆 0");

    setupScene();
    loadRoom(0);

    countdownTimer->start();
    loopTimer->start();
    this->setFocus();
}


// ═══════════════════════════════════════════════════════
// RESET GAME  — stops everything cleanly
// ═══════════════════════════════════════════════════════

void MainWindow::resetGame()
{
    loopTimer->stop();
    countdownTimer->stop();
    gameActive  = false;
    riddleOpen  = false;
    heldKeys.clear();

    clearRoom();

    if (playerSprite && scene) {
        scene->removeItem(playerSprite);
        delete playerSprite;
        playerSprite = nullptr;
    }
}


// ═══════════════════════════════════════════════════════
// SCENE SETUP  — creates the QGraphicsScene once
// ═══════════════════════════════════════════════════════

void MainWindow::setupScene()
{
    if (!scene) {
        scene = new QGraphicsScene(0, 0, 891, 421, this);
        scene->setSceneRect(0, 0, 891, 421);
        ui->graphicsView->setScene(scene);
    } else {
        clearRoom();
    }

    // (Re)create player sprite with selected character portrait
    if (playerSprite) {
        scene->removeItem(playerSprite);
        delete playerSprite;
        playerSprite = nullptr;
    }

    playerSprite = new QGraphicsPixmapItem(createPlayerSprite());
    playerSprite->setZValue(10);
    playerSprite->setPos(20, 173);
    scene->addItem(playerSprite);
}


// ═══════════════════════════════════════════════════════
// CLEAR ROOM  — removes all room objects (not the player)
// ═══════════════════════════════════════════════════════

void MainWindow::clearRoom()
{
    // Crescents are scene items not tracked in roomItems
    for (auto &c : crescents) {
        if (scene) scene->removeItem(c.item);
        delete c.item;
    }
    crescents.clear();

    // Boss sprite is also outside roomItems
    if (bossSprite && scene) {
        scene->removeItem(bossSprite);
        delete bossSprite;
        bossSprite = nullptr;
    }
    bossState = BossState::INACTIVE;

    // Clear destructible covers
    for (auto &dc : destructibleCovers) {
        if (dc.item && scene) { scene->removeItem(dc.item); delete dc.item; }
    }
    destructibleCovers.clear();

    for (auto &t : traps) {
        if (scene) scene->removeItem(t.item);
        delete t.item;
    }
    traps.clear();

    // Clear clue fragments
    for (auto &cf : clueFragments) {
        if (cf.sprite && scene) { scene->removeItem(cf.sprite); delete cf.sprite; }
        if (cf.glowEffect && scene) { scene->removeItem(cf.glowEffect); delete cf.glowEffect; }
    }
    clueFragments.clear();

    // Clear statues
    for (auto &s : statues) {
        if (s.sprite && scene) { scene->removeItem(s.sprite); delete s.sprite; }
        if (s.hitbox && scene) { scene->removeItem(s.hitbox); delete s.hitbox; }
    }
    statues.clear();

    // Clear tablets
    for (auto &tb : tablets) {
        if (tb.sprite && scene) { scene->removeItem(tb.sprite); delete tb.sprite; }
        if (tb.hitbox && scene) { scene->removeItem(tb.hitbox); delete tb.hitbox; }
    }
    tablets.clear();

    // Clear torches
    for (auto &tr : torches) {
        if (tr.sprite && scene) { scene->removeItem(tr.sprite); delete tr.sprite; }
        if (tr.hitbox && scene) { scene->removeItem(tr.hitbox); delete tr.hitbox; }
    }
    torches.clear();

    // Clear relics
    for (auto &r : relics) {
        if (r.sprite && scene) { scene->removeItem(r.sprite); delete r.sprite; }
    }
    relics.clear();

    // Clear altar
    if (altar.sprite && scene) { scene->removeItem(altar.sprite); delete altar.sprite; }
    if (altar.hitbox && scene) { scene->removeItem(altar.hitbox); delete altar.hitbox; }
    if (altar.glowEffect && scene) { scene->removeItem(altar.glowEffect); delete altar.glowEffect; }
    if (altar.hint && scene) { scene->removeItem(altar.hint); delete altar.hint; }
    altar = {nullptr, nullptr, nullptr, nullptr, false};

    // Clear room-specific mechanics
    for (auto *fd : fakeDoors) {
        if (scene) scene->removeItem(fd);
        delete fd;
    }
    fakeDoors.clear();
    realDoor = nullptr;

    for (auto *sp : spikeTraps) {
        if (scene) scene->removeItem(sp);
        delete sp;
    }
    spikeTraps.clear();
    spikeActive.clear();

    for (auto *ct : collapsingTiles) {
        if (scene) scene->removeItem(ct);
        delete ct;
    }
    collapsingTiles.clear();
    collapseTriggerTimer.clear();
    tileCollapsed.clear();

    // Reset per-room clue counter
    cluesFound = 0;

    for (QGraphicsItem *item : roomItems) {
        if (scene) scene->removeItem(item);
        delete item;
    }
    roomItems.clear();
    obstacles.clear();

    doorItem   = nullptr;
    pressEHint = nullptr;
    bossHPBar  = nullptr;
}


// ═══════════════════════════════════════════════════════
// LOAD ROOM  — builds scene for one room, updates HUD
// ═══════════════════════════════════════════════════════

void MainWindow::loadRoom(int room)
{
    currentRoom = room;
    clearRoom();
    if (playerSprite) playerSprite->setPos(20, 173);

    // Show story intro for each room
    showRoomIntro(room);

    // Background colour per room (7 rooms: 0–5 normal + 5 boss + 6 Anubis)
    static const QColor BG[7] = {
        {45,32,12}, {28,22,18}, {12,4,28},
        {4,12,32},  {38,24,4},  {5,3,22},
        {4,2,10}
    };
    scene->setBackgroundBrush(QBrush(BG[room]));

    // Floor strip
    addRoomRect(0, 360, 891, 61, QColor(0,0,0,80));

    // Set required clues per room (boss room doesn't need clues)
    requiredClues = (room == 5) ? 0 : 3;

    // ═══════════════════════════════════════════════════════
    // ROOM 0: ENTRANCE CHAMBER — Tutorial / exploration intro
    // Mechanic: TUTORIAL (no enemies, learn the ropes)
    // ═══════════════════════════════════════════════════════
    if (room == 0) {
        currentMechanic = RoomMechanic::TUTORIAL;

        // Decorative pillars creating side paths
        addRoomRect(140, 40, 30, 150, QColor(110,85,45), QColor(150,120,70), 1.5, 1);
        addRoomRect(140, 220, 30, 130, QColor(110,85,45), QColor(150,120,70), 1.5, 1);
        addRoomRect(680, 40, 30, 130, QColor(110,85,45), QColor(150,120,70), 1.5, 1);
        addRoomRect(680, 200, 30, 150, QColor(110,85,45), QColor(150,120,70), 1.5, 1);

        // Center chamber area
        addRoomRect(350, 100, 180, 180, QColor(60,45,25,100), QColor(100,80,50), 2, 1);
        addRoomText("ENTRANCE", 380, 15, QColor(180,150,80,160), 18, 1);

        // Obstacles to create exploration paths
        addObstacle(250, 80, 40, 80, 0);
        addObstacle(550, 200, 50, 90, 0);
        addObstacle(300, 280, 45, 60, 0);

        // Clue fragments scattered around
        addClueFragment(100, 60);      // top-left corner
        addClueFragment(600, 300);     // bottom-right area
        addClueFragment(420, 140);     // center chamber
        addClueFragment(200, 250);     // left alcove (bonus)

        // Statues with lore
        addStatue(85, 270, "The statue depicts Ra, the Sun God. 'Seek wisdom in shadows.'");
        addStatue(750, 80, "An ancient pharaoh stands guard. 'Only the worthy may pass.'");

        // Tablet with hint
        addTablet(440, 280, "Tablet reads: 'Echoes speak without voice...'");

        // Torch (reveals nothing special in tutorial)
        addTorch(180, 150);

        // Central altar
        addAltarToRoom(390, 180);

        // No enemies in tutorial
        ui->controlsLabel->setText("WASD Move | E Interact | Collect 3 clues to activate altar");
    }

    // ═══════════════════════════════════════════════════════
    // ROOM 1: GUARDIAN HALL — Stealth mechanic
    // Mechanic: STEALTH_GUARDIAN (large detection radius, must sneak)
    // ═══════════════════════════════════════════════════════
    else if (room == 1) {
        currentMechanic = RoomMechanic::STEALTH_GUARDIAN;

        // Tall pillars for cover
        addRoomRect(200, 20, 24, 340, QColor(70,60,50), QColor(100,90,80), 1, 1);
        addRoomRect(450, 20, 24, 340, QColor(70,60,50), QColor(100,90,80), 1, 1);
        addRoomRect(650, 20, 24, 340, QColor(70,60,50), QColor(100,90,80), 1, 1);
        addRoomText("GUARDIAN HALL", 290, 340, QColor(160,130,70,180), 16, 1);

        // Cover obstacles
        addObstacle(120, 60, 50, 70, 0);
        addObstacle(320, 180, 60, 50, 0);
        addObstacle(550, 80, 45, 80, 0);
        addObstacle(350, 290, 55, 50, 0);
        addObstacle(580, 260, 40, 70, 0);

        // Stealth guardian — moderate detection, manageable chase
        addTrap(400, 140, 90, 120, true, 160, 680, 1.8f, 1.f, 0, 90.f, 2.5f);

        // Clue fragments in cover positions
        addClueFragment(90, 280);      // behind cover left
        addClueFragment(520, 50);      // top right corner
        addClueFragment(720, 300);     // far right
        addClueFragment(280, 100);     // between pillars

        // Statues
        addStatue(140, 180, "A colossal guardian. 'Move when its gaze is elsewhere.'");

        // Tablets
        addTablet(600, 180, "Carved warning: 'The guardian sees all who rush.'");
        addTablet(250, 300, "Hint: 'Footsteps leave traces behind...'");

        // Torch in dark corner
        addTorch(85, 40);

        // Hidden relic (visible)
        addRelic(750, 150, false);

        // Central altar
        addAltarToRoom(380, 200);

        ui->controlsLabel->setText("WASD Move | E Interact | STEALTH: Stay behind cover!");
    }

    // ═══════════════════════════════════════════════════════
    // ROOM 2: HALL OF ECHOES — Fake doors mechanic
    // Mechanic: FAKE_DOORS (2 fake doors damage, 1 real altar)
    // ═══════════════════════════════════════════════════════
    else if (room == 2) {
        currentMechanic = RoomMechanic::FAKE_DOORS;

        // Ethereal atmosphere
        for (int i = 0; i < 10; i++) {
            addRoomRect(80 + i * 85, 30 + (i % 4) * 75, 10, 10,
                        QColor(180, 100, 255, 80 + i * 15),
                        QColor(220, 160, 255, 150), 1, 1);
        }
        addRoomText("HALL OF ECHOES", 275, 340, QColor(180,100,255,180), 16, 1);

        // Obstacles creating maze-like paths
        addObstacle(150, 50, 30, 120, 0);
        addObstacle(280, 150, 35, 140, 0);
        addObstacle(450, 60, 30, 130, 0);
        addObstacle(580, 180, 35, 130, 0);
        addObstacle(350, 280, 100, 30, 0);

        // Fake doors (touching damages player)
        addFakeDoor(200, 200);
        addFakeDoor(550, 80);

        // Spectres patrol only (no chase)
        addTrap(320, 20, 55, 80, false, 20, 290, 2.2f, 1.f, 1, 0.f, 0.f);
        addTrap(620, 120, 55, 80, false, 100, 320, 2.5f, -1.f, 1, 0.f, 0.f);

        // Clue fragments
        addClueFragment(100, 200);
        addClueFragment(400, 100);
        addClueFragment(700, 280);
        addClueFragment(180, 320);

        // Statues
        addStatue(85, 60, "A ghostly figure. 'Not all paths lead forward.'");

        // Tablets
        addTablet(680, 40, "Glyphs shimmer: 'The true path reveals itself to the patient.'");
        addTablet(130, 260, "Warning: 'Illusions guard this hall...'");

        // Torch that reveals which door is fake (visual hint)
        addTorch(120, 320);

        // Hidden relic
        addRelic(750, 320, false);

        // Central altar (the REAL goal)
        addAltarToRoom(360, 150);

        ui->controlsLabel->setText("WASD Move | E Interact | BEWARE: Some doors are traps!");
    }

    // ═══════════════════════════════════════════════════════
    // ROOM 3: TRIAL ROOM — Spike trap timing mechanic
    // Mechanic: SPIKE_TRAPS (timed floor spikes)
    // ═══════════════════════════════════════════════════════
    else if (room == 3) {
        currentMechanic = RoomMechanic::SPIKE_TRAPS;
        spikeTimer = 0;

        // Trial room decorations
        for (int i = 0; i < 6; i++) {
            addRoomRect(100 + i * 130, 345, 90, 12, QColor(60, 80, 180, 120),
                        QColor(100, 140, 255, 180), 1, 1);
        }
        addRoomText("TRIAL ROOM", 350, 15, QColor(100,140,255,180), 18, 1);

        // Spike trap zones (smaller, alternating pattern)
        addSpikeTrap(130, 90, 70, 40);
        addSpikeTrap(330, 90, 70, 40);
        addSpikeTrap(530, 90, 70, 40);
        addSpikeTrap(230, 190, 70, 40);
        addSpikeTrap(430, 190, 70, 40);
        addSpikeTrap(630, 190, 70, 40);
        addSpikeTrap(130, 290, 70, 40);
        addSpikeTrap(330, 290, 70, 40);
        addSpikeTrap(530, 290, 70, 40);

        // Safe obstacle platforms
        addObstacle(180, 130, 50, 40, 0);
        addObstacle(380, 130, 50, 40, 0);
        addObstacle(580, 130, 50, 40, 0);
        addObstacle(280, 230, 50, 40, 0);
        addObstacle(480, 230, 50, 40, 0);

        // Mummies patrol only (no chase)
        addTrap(200, 150, 70, 60, true, 100, 650, 2.0f, 1.f, 0, 0.f, 0.f);
        addTrap(400, 260, 70, 60, true, 100, 650, 2.2f, -1.f, 0, 0.f, 0.f);

        // Clue fragments on safe spots
        addClueFragment(90, 170);
        addClueFragment(720, 100);
        addClueFragment(300, 320);
        addClueFragment(550, 300);

        // Statues
        addStatue(85, 40, "Ancient mechanism. 'Patience conquers the trial.'");

        // Tablets
        addTablet(720, 260, "Instructions: 'Watch the rhythm of the stones.'");

        // Torch
        addTorch(680, 50);

        // Relic (visible, risky to get)
        addRelic(720, 180, false);

        // Altar in center
        addAltarToRoom(350, 160);

        ui->controlsLabel->setText("WASD Move | E Interact | TIME the spikes carefully!");
    }

    // ═══════════════════════════════════════════════════════
    // ROOM 4: ANKH SANCTUARY — Hidden relic puzzle
    // Mechanic: HIDDEN_RELIC_PUZZLE (torch reveals relic)
    // ═══════════════════════════════════════════════════════
    else if (room == 4) {
        currentMechanic = RoomMechanic::HIDDEN_RELIC_PUZZLE;

        // Golden sanctuary decorations
        addRoomRect(720, 80, 100, 20, QColor(200, 160, 40, 180), QColor(255, 210, 80), 2, 1);
        addRoomRect(740, 40, 60, 45, QColor(220, 180, 60, 140), QColor(255, 220, 100), 2, 1);
        addRoomText("ANKH SANCTUARY", 285, 340, QColor(200,160,50,180), 16, 1);

        // Maze-like sanctuary layout
        addObstacle(150, 40, 40, 150, 1);
        addObstacle(300, 120, 45, 180, 1);
        addObstacle(480, 40, 40, 140, 1);
        addObstacle(600, 150, 45, 160, 1);
        addObstacle(200, 280, 200, 35, 1);

        // Scarabs patrol only (no chase, stay away from altar area)
        addTrap(120, 280, 65, 65, true, 100, 320, 2.2f, 1.f, 2, 0.f, 0.f);
        addTrap(550, 60, 65, 65, false, 40, 140, 2.5f, 1.f, 2, 0.f, 0.f);

        // Clue fragments
        addClueFragment(100, 80);
        addClueFragment(550, 100);
        addClueFragment(250, 200);
        addClueFragment(700, 300);

        // Statues
        addStatue(85, 280, "The Ankh. 'Life eternal awaits the worthy.'");
        addStatue(650, 40, "Guardian of the sanctuary. 'Light reveals truth.'");

        // Tablets
        addTablet(350, 50, "Inscribed: 'The sacred relic hides in darkness.'");

        // Important torch — reveals the hidden relic
        addTorch(85, 60);

        // Hidden relic (NOT visible until torch lit)
        addRelic(750, 250, false);

        // Altar
        addAltarToRoom(380, 180);

        ui->controlsLabel->setText("WASD Move | E Interact | Light the torch to reveal secrets!");
    }

    // ═══════════════════════════════════════════════════════
    // ROOM 5: KHONSHU'S CHAMBER — Boss fight with destructible cover
    // ═══════════════════════════════════════════════════════
    else if (room == 5) {
        currentMechanic = RoomMechanic::BOSS_FIGHT;

        // Moonlit atmosphere: scattered star particles
        for (int i = 0; i < 14; i++) {
            int sx = (i * 61 + 30) % 820;
            int sy = (i * 43 + 15) % 320;
            int sr = 2 + i % 4;
            addRoomRect(sx, sy, sr * 2, sr * 2,
                        QColor(200, 185, 255, 55 + i * 10));
        }

        addRoomText("KHONSHU'S CHAMBER", 240, 340,
                    QColor(175, 135, 255, 210), 16, 1);

        // Destructible cover blocks (break after 5 crescent hits)
        auto addCover = [&](qreal x, qreal y, qreal w, qreal h) {
            auto *cover = scene->addRect(x, y, w, h,
                                          QPen(QColor(120, 90, 180), 2),
                                          QBrush(QColor(80, 60, 140, 220)));
            cover->setZValue(3);
            destructibleCovers.append({cover, 5});
        };
        addCover(150, 80, 50, 50);
        addCover(150, 200, 50, 50);
        addCover(150, 280, 50, 50);
        addCover(350, 120, 50, 50);
        addCover(350, 240, 50, 50);

        // Boss HP bar
        addRoomRect(326, 4, 206, 18, QColor(28, 10, 10, 210),
                    QColor(140, 50, 50), 2, 15);
        bossHPBar = addRoomRect(328, 6, 200, 14,
                                QColor(190, 42, 42, 230), Qt::transparent, 0, 16);
        addRoomText("KHONSHU HP", 232, 4, QColor(205, 155, 255, 230), 12, 15);

        // Boss sprite
        bossSprite = new QGraphicsPixmapItem(createKhonshuSprite(120, 200));
        bossSprite->setPos(590, 90);
        bossSprite->setZValue(6);
        scene->addItem(bossSprite);

        // Initialize boss state
        bossState         = BossState::SHOOTING;
        bossHP            = 25;
        bossShootCount    = 0;
        bossShootTimer    = 0;
        bossTiredTimer    = 0;
        bossHitsThisPhase = 0;
        bossDyingTimer    = 0;

        ui->controlsLabel->setText(
            "WASD Move | R Strike Boss (when glowing) | ESC Pause");
    }

    // ═══════════════════════════════════════════════════════
    // ROOM 6: ANUBIS GATE — Collapsing floor mechanic
    // Mechanic: COLLAPSING_FLOOR (floor crumbles after stepping)
    // ═══════════════════════════════════════════════════════
    else if (room == 6) {
        currentMechanic = RoomMechanic::COLLAPSING_FLOOR;

        // Final gate atmosphere
        addRoomRect(760, 0, 131, 420, QColor(15, 8, 30, 200), QColor(80, 40, 120), 2, 1);
        addRoomRect(790, 20, 70, 140, QColor(30, 10, 60, 220), QColor(120, 60, 180), 2, 1);
        addRoomText("ANUBIS GATE", 330, 340, QColor(120,60,200,200), 18, 1);

        // Collapsing floor tiles (smaller)
        addCollapsingTile(120, 60, 55, 55);
        addCollapsingTile(280, 60, 55, 55);
        addCollapsingTile(440, 60, 55, 55);
        addCollapsingTile(200, 160, 55, 55);
        addCollapsingTile(360, 160, 55, 55);
        addCollapsingTile(520, 160, 55, 55);
        addCollapsingTile(120, 260, 55, 55);
        addCollapsingTile(280, 260, 55, 55);
        addCollapsingTile(440, 260, 55, 55);

        // Safe stone platforms
        addObstacle(600, 80, 50, 60, 2);
        addObstacle(600, 200, 50, 60, 2);
        addObstacle(600, 300, 50, 60, 2);

        // Dark mummies (no chase - patrol only)
        addTrap(300, 120, 60, 60, true, 100, 550, 2.8f, 1.f, 3, 0.f, 0.f);
        addTrap(400, 240, 60, 60, true, 100, 550, 3.0f, -1.f, 3, 0.f, 0.f);

        // Clue fragments
        addClueFragment(90, 100);
        addClueFragment(550, 280);
        addClueFragment(350, 50);
        addClueFragment(180, 300);

        // Statues
        addStatue(700, 260, "Anubis awaits. 'Judge of the dead.'");

        // Tablets
        addTablet(85, 200, "Final warning: 'The floor hungers for the unworthy.'");

        // Torch
        addTorch(85, 320);

        // Final relic (risky placement)
        addRelic(500, 100, false);

        // Altar for final riddle
        addAltarToRoom(650, 170);

        ui->controlsLabel->setText("WASD Move | E Interact | CAREFUL: The floor collapses!");
    }

    // Doors removed - altar system handles room transitions
    doorItem = nullptr;
    pressEHint = nullptr;

    // Update HUD
    ui->roomNameLabel->setText(ROOMS[room].name);
    setEventText(ROOMS[room].enterText);
    ui->levelLabel->setText(QString("%1 / 7").arg(room + 1));
    refreshMap();
}


// ═══════════════════════════════════════════════════════
// TRY INTERACT  — E key handler (checks all interactables)
// ═══════════════════════════════════════════════════════

void MainWindow::tryInteract()
{
    if (!gameActive || riddleOpen || !playerSprite) return;

    QPointF pc = playerSprite->sceneBoundingRect().center();
    const float interactRadius = 70.f;

    // Priority 1: Clue fragments
    for (auto &cf : clueFragments) {
        if (cf.collected || !cf.sprite) continue;
        QPointF cc = cf.sprite->sceneBoundingRect().center();
        float dist = qSqrt(qPow(pc.x() - cc.x(), 2) + qPow(pc.y() - cc.y(), 2));
        if (dist < interactRadius) {
            tryCollectClue();
            return;
        }
    }

    // Priority 2: Statues
    for (auto &s : statues) {
        if (!s.hitbox) continue;
        QPointF sc = s.hitbox->sceneBoundingRect().center();
        float dist = qSqrt(qPow(pc.x() - sc.x(), 2) + qPow(pc.y() - sc.y(), 2));
        if (dist < interactRadius) {
            tryInteractStatue();
            return;
        }
    }

    // Priority 3: Tablets
    for (auto &tb : tablets) {
        if (!tb.hitbox) continue;
        QPointF tc = tb.hitbox->sceneBoundingRect().center();
        float dist = qSqrt(qPow(pc.x() - tc.x(), 2) + qPow(pc.y() - tc.y(), 2));
        if (dist < interactRadius) {
            tryInteractTablet();
            return;
        }
    }

    // Priority 4: Torches
    for (auto &tr : torches) {
        if (!tr.hitbox) continue;
        QPointF trc = tr.hitbox->sceneBoundingRect().center();
        float dist = qSqrt(qPow(pc.x() - trc.x(), 2) + qPow(pc.y() - trc.y(), 2));
        if (dist < interactRadius) {
            tryInteractTorch();
            return;
        }
    }

    // Priority 5: Relics
    for (auto &r : relics) {
        if (r.collected || !r.visible || !r.sprite) continue;
        QPointF rc = r.sprite->sceneBoundingRect().center();
        float dist = qSqrt(qPow(pc.x() - rc.x(), 2) + qPow(pc.y() - rc.y(), 2));
        if (dist < interactRadius) {
            tryCollectRelic();
            return;
        }
    }

    // Priority 6: Altar (only if active)
    if (altar.active && altar.hitbox) {
        QPointF ac = altar.hitbox->sceneBoundingRect().center();
        float dist = qSqrt(qPow(pc.x() - ac.x(), 2) + qPow(pc.y() - ac.y(), 2));
        if (dist < 100.f) {
            showRiddle();
            return;
        }
    }

}


// ═══════════════════════════════════════════════════════
// SHOW RIDDLE
// ═══════════════════════════════════════════════════════

void MainWindow::showRiddle()
{
    riddleOpen = true;
    loopTimer->stop();
    countdownTimer->stop();
    heldKeys.clear();

    RiddleDialog dlg(RIDDLES[currentRoom], selectedTrait, this);
    dlg.exec();

    if (!gameActive) return;   // reset() was called while dialog was open

    countdownTimer->start();
    loopTimer->start();
    riddleOpen = false;

    if (dlg.wasCorrect())
        onRiddleCorrect();
    else
        onRiddleWrong();
}


// ═══════════════════════════════════════════════════════
// RIDDLE CORRECT
// ═══════════════════════════════════════════════════════

void MainWindow::onRiddleCorrect()
{
    const RoomInfo &r = ROOMS[currentRoom];
    int earned = r.baseScore;

    if      (selectedTrait == "Scholar")   earned += r.bonusScore;
    else if (selectedTrait == "Survivor")  earned += r.bonusScore - 10;
    else if (selectedTrait == "Speedster") {
        earned += r.bonusScore - 10;
        timeSeconds += 30;
        refreshTimer();
    }

    float mult = 1.f;
    if (selectedDifficulty == Difficulty::MEDIUM) mult = 1.5f;
    else if (selectedDifficulty == Difficulty::HARD) mult = 2.f;

    score += int(earned * mult);
    ui->scoreLabel->setText(QString("🏆 %1").arg(score));

    // Survivor heals in Ankh room
    if (r.survivorHeals && selectedTrait == "Survivor" && lives < maxLives) {
        lives++;
        refreshLives();
    }

    setEventText(r.solvedText);

    if (currentRoom == 6) {
        triggerWin();
    } else {
        // Immediately advance to next room
        loadRoom(currentRoom + 1);
    }
}


// ═══════════════════════════════════════════════════════
// RIDDLE WRONG
// ═══════════════════════════════════════════════════════

void MainWindow::onRiddleWrong()
{
    lives--;
    refreshLives();

    if (lives <= 0) {
        triggerGameOver("You failed the ancient trial. The tomb claims you.");
        return;
    }

    if (playerSprite) playerSprite->setPos(20, 173);
    setEventText("Wrong! The tomb punishes you.\nWalk back to the door and press E to try again.");
}


// ═══════════════════════════════════════════════════════
// TRAP HIT
// ═══════════════════════════════════════════════════════

void MainWindow::onTrapHit()
{
    if (invincible) return;

    // Survivor: 50 % chance to resist
    if (selectedTrait == "Survivor" &&
        QRandomGenerator::global()->bounded(100) < 50) {
        invincible = true;
        invFrames  = 90;
        setEventText("The trap strikes — your endurance holds! (Survivor)");
        return;
    }

    lives--;
    refreshLives();
    invincible = true;
    invFrames  = 120;   // 2 s invincibility

    if (lives <= 0)
        triggerGameOver("The tomb's traps have consumed you.");
    else
        setEventText(QString("A trap strikes you! Lives remaining: %1").arg(lives));
}


// ═══════════════════════════════════════════════════════
// GAME OVER / WIN
// ═══════════════════════════════════════════════════════

void MainWindow::triggerGameOver(const QString &reason)
{
    loopTimer->stop();
    countdownTimer->stop();
    gameActive = false;

    ui->gameOverCauseLabel->setText(reason);
    ui->label_2->setText(QString::number(score));
    ui->stackedWidget->setCurrentWidget(ui->gameOverPage);
}

void MainWindow::triggerWin()
{
    loopTimer->stop();
    countdownTimer->stop();
    gameActive = false;

    score += timeSeconds;   // time-remaining bonus
    ui->winScoreLabel->setText(QString::number(score));
    ui->stackedWidget->setCurrentWidget(ui->winPage);
}


// ═══════════════════════════════════════════════════════
// SCENE HELPERS
// ═══════════════════════════════════════════════════════

QGraphicsRectItem* MainWindow::addRoomRect(qreal x, qreal y, qreal w, qreal h,
                                            QColor fill, QColor border,
                                            qreal bw, int z)
{
    QPen pen(border, bw);
    if (border == Qt::transparent) pen.setStyle(Qt::NoPen);
    auto *item = scene->addRect(x, y, w, h, pen, QBrush(fill));
    item->setZValue(z);
    roomItems.append(item);
    return item;
}

QGraphicsTextItem* MainWindow::addRoomText(const QString &txt, qreal x, qreal y,
                                             QColor col, int sz, int z)
{
    auto *item = scene->addText(txt);
    item->setDefaultTextColor(col);
    QFont f; f.setPixelSize(sz);
    item->setFont(f);
    item->setPos(x, y);
    item->setZValue(z);
    roomItems.append(item);
    return item;
}

void MainWindow::addTrap(qreal x, qreal y, qreal w, qreal h,
                          bool horiz, float mn, float mx, float spd,
                          float startDir, int enemyType,
                          float detectRadius, float chaseSpd)
{
    // Invisible rect used purely for collision / movement bookkeeping
    auto *item = new QGraphicsRectItem(0, 0, w, h);
    item->setPen(Qt::NoPen);
    item->setBrush(Qt::NoBrush);
    item->setPos(x, y);
    item->setZValue(5);
    scene->addItem(item);

    // Choose sprite based on enemy type
    QPixmap spr;
    switch (enemyType) {
    case 1: spr = createSpecterSprite(int(w), int(h));      break;  // specter
    case 2: spr = createScarabSprite (int(w), int(h));      break;  // scarab
    case 3: spr = createMummySprite  (int(w), int(h), true); break; // dark mummy
    default: spr = createMummySprite (int(w), int(h));      break;  // mummy
    }

    auto *sprite = new QGraphicsPixmapItem(spr, item);
    sprite->setPos(0, 0);

    // Store home position for chase return behavior
    float homeX = float(x) + float(w) / 2.f;
    float homeY = float(y) + float(h) / 2.f;

    traps.append({item, sprite, horiz, mn, mx, spd, startDir, enemyType,
                  detectRadius, chaseSpd > 0 ? chaseSpd : spd * 1.5f, false, homeX, homeY});
}

void MainWindow::addObstacle(qreal x, qreal y, qreal w, qreal h, int style)
{
    QColor fill, border;
    switch (style) {
    case 1: // golden altar stone
        fill   = QColor(138, 102, 34, 245);
        border = QColor(192, 152, 55);
        break;
    case 2: // cursed dark stone
        fill   = QColor(36, 26, 50, 248);
        border = QColor(72, 46, 108);
        break;
    default: // sandy desert stone
        fill   = QColor(90, 74, 50, 248);
        border = QColor(122, 100, 66);
        break;
    }

    auto *item = scene->addRect(x, y, w, h, QPen(border, 2), QBrush(fill));
    item->setZValue(3);
    roomItems.append(item);
    obstacles.append(item);

    // Carved diamond mark in the centre of larger blocks
    if (w >= 22 && h >= 22) {
        int mx = int(x) + int(w) / 2 - 4, my = int(y) + int(h) / 2 - 4;
        auto *mark = scene->addRect(mx, my, 8, 8,
                                     QPen(border.darker(145), 1),
                                     QBrush(fill.darker(140)));
        mark->setZValue(4);
        roomItems.append(mark);
    }
}


// ═══════════════════════════════════════════════════════
// HUD HELPERS
// ═══════════════════════════════════════════════════════

void MainWindow::refreshTimer()
{
    int m = timeSeconds / 60, s = timeSeconds % 60;
    ui->timerLabel->setText(QString("%1:%2")
        .arg(m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0')));
}

void MainWindow::refreshLives()
{
    QString h;
    for (int i = 0; i < lives; i++) h += "♥ ";
    ui->livesLabel->setText(h.isEmpty() ? "—" : h.trimmed());
}

void MainWindow::refreshMap()
{
    // 7 rooms but only 6 sidebar slots (rooms 0–5).
    // Room 6 (Anubis Gate) has no slot; show all 6 as done when there.
    for (int i = 0; i < 6; i++) {
        bool allDone = (currentRoom > 5);
        if      (allDone || i < currentRoom)  mapLabels[i]->setStyleSheet(S_DONE);
        else if (i == currentRoom)            mapLabels[i]->setStyleSheet(S_ACTIVE);
        else                                  mapLabels[i]->setStyleSheet(S_NORMAL);
    }
}

void MainWindow::setEventText(const QString &msg)
{
    ui->eventLabel->setText(msg);
}


// ═══════════════════════════════════════════════════════
// BOSS FIGHT — Khonshu (room 5)
// State machine: SHOOTING (8 crescents) → TIRED (10 s) → SHOOTING → …
// Player hits with R key during TIRED phase; max 5 hits per opening.
// 25 total hits to kill the boss.
// ═══════════════════════════════════════════════════════

void MainWindow::updateBoss()
{
    if (!bossSprite || bossState == BossState::INACTIVE) return;

    switch (bossState) {

    case BossState::SHOOTING:
        // Fire one crescent every 45 frames (~0.75 s at 60 fps)
        bossShootTimer++;
        if (bossShootTimer >= 45) {
            bossShootTimer = 0;
            fireCrescent();
            bossShootCount++;
            if (bossShootCount >= 8) {
                // Transition to TIRED after 8 crescents
                bossState         = BossState::TIRED;
                bossTiredTimer    = 600;   // 10 seconds
                bossHitsThisPhase = 0;
                bossShootCount    = 0;
                setEventText("Khonshu is exhausted! Move close and press R to strike! (max 5 hits)");
            }
        }
        // Subtle float animation: oscillate opacity slightly
        if (bossSprite)
            bossSprite->setOpacity(0.92 + 0.08 * qSin(bossShootTimer * 0.15));
        break;

    case BossState::TIRED:
    {
        bossTiredTimer--;
        // Pulse to signal vulnerability
        double pulse = 0.45 + 0.40 * qSin(bossTiredTimer * 0.22);
        bossSprite->setOpacity(pulse);

        // 3-second warning before resuming
        if (bossTiredTimer == 180)
            setEventText("Khonshu is stirring — land your final hits now!");

        if (bossTiredTimer <= 0) {
            bossState = BossState::SHOOTING;
            bossShootTimer = 0;
            bossSprite->setOpacity(1.0);
            setEventText("Khonshu rises again! Dodge the crescents!");
        }
        break;
    }

    case BossState::DYING:
        bossDyingTimer--;
        if (bossSprite)
            bossSprite->setOpacity(qMax(0.0, double(bossDyingTimer) / 90.0));
        if (bossDyingTimer <= 0) {
            bossState = BossState::INACTIVE;
            // Transition to the final room
            score += ROOMS[5].baseScore;
            ui->scoreLabel->setText(QString("🏆 %1").arg(score));
            loadRoom(6);
        }
        break;

    case BossState::INACTIVE:
        break;
    }
}

void MainWindow::fireCrescent()
{
    if (!playerSprite || !bossSprite) return;

    QPointF bossCenter   = bossSprite->sceneBoundingRect().center();
    QPointF playerCenter = playerSprite->sceneBoundingRect().center();

    float dx = float(playerCenter.x() - bossCenter.x());
    float dy = float(playerCenter.y() - bossCenter.y());
    float len = qSqrt(dx * dx + dy * dy);
    if (len < 1.f) len = 1.f;

    const float speed = 3.5f;
    float vx = dx / len * speed;
    float vy = dy / len * speed;

    auto *item = new QGraphicsPixmapItem(createCrescentSprite(22, 22));
    item->setPos(bossCenter.x() - 11, bossCenter.y() - 11);
    item->setZValue(8);
    scene->addItem(item);

    crescents.append({item, vx, vy});
}

void MainWindow::updateCrescents()
{
    if (!playerSprite) return;

    const QRectF sceneRect(0, 0, 891, 421);
    QRectF playerRect = playerSprite->sceneBoundingRect();

    for (int i = crescents.size() - 1; i >= 0; i--) {
        Crescent &c = crescents[i];
        QPointF pos = c.item->pos();
        pos.rx() += c.vx;
        pos.ry() += c.vy;
        c.item->setPos(pos);

        // Remove crescent when it leaves the scene
        if (!sceneRect.intersects(c.item->sceneBoundingRect())) {
            scene->removeItem(c.item);
            delete c.item;
            crescents.remove(i);
            continue;
        }

        // Check collision with destructible covers
        bool hitCover = false;
        for (int j = destructibleCovers.size() - 1; j >= 0; j--) {
            auto &dc = destructibleCovers[j];
            if (dc.item && c.item->sceneBoundingRect().intersects(dc.item->sceneBoundingRect())) {
                // Damage cover
                dc.hp--;
                if (dc.hp <= 0) {
                    // Cover destroyed
                    scene->removeItem(dc.item);
                    delete dc.item;
                    destructibleCovers.remove(j);
                } else {
                    // Visual feedback - fade cover
                    int alpha = 220 - (5 - dc.hp) * 35;
                    dc.item->setBrush(QBrush(QColor(80, 60, 140, alpha)));
                }
                // Remove crescent
                scene->removeItem(c.item);
                delete c.item;
                crescents.remove(i);
                hitCover = true;
                break;
            }
        }
        if (hitCover) continue;

        // Crescent hits player (only when not already invincible)
        if (!invincible && playerRect.intersects(c.item->sceneBoundingRect())) {
            scene->removeItem(c.item);
            delete c.item;
            crescents.remove(i);
            onTrapHit();   // reuses existing heart/invincibility logic
        }
    }
}

void MainWindow::tryBossHit()
{
    if (!gameActive || bossState != BossState::TIRED || !bossSprite || !playerSprite)
        return;

    if (bossHitsThisPhase >= 5) {
        setEventText("Maximum hits reached for this phase — dodge until the next opening!");
        return;
    }

    QPointF pc = playerSprite->sceneBoundingRect().center();
    QPointF bc = bossSprite->sceneBoundingRect().center();
    float dist = qSqrt(qPow(pc.x() - bc.x(), 2) + qPow(pc.y() - bc.y(), 2));

    if (dist > 160.f) {
        setEventText("Move closer to Khonshu and press R to strike!");
        return;
    }

    bossHitsThisPhase++;
    bossHP--;

    // Flash the boss white briefly
    bossSprite->setOpacity(0.15);

    // Shrink HP bar proportionally
    if (bossHPBar) {
        float ratio = float(bossHP) / 25.0f;
        bossHPBar->setRect(328, 6, qMax(0.f, 200.f * ratio), 14);
    }

    if (bossHP <= 0) {
        bossState      = BossState::DYING;
        bossDyingTimer = 90;
        // Clear all projectiles immediately
        for (auto &c : crescents) {
            scene->removeItem(c.item);
            delete c.item;
        }
        crescents.clear();
        setEventText("KHONSHU FALLS! The Moon God crumbles into dust...");
    } else if (bossHitsThisPhase >= 5) {
        // Used all 5 hits - end tired phase immediately
        bossState = BossState::SHOOTING;
        bossShootTimer = 0;
        bossSprite->setOpacity(1.0);
        setEventText("Khonshu recovers! Dodge the crescents!");
    } else {
        setEventText(QString("Hit! Khonshu HP: %1 | Hits this phase: %2 / 5")
                     .arg(bossHP).arg(bossHitsThisPhase));
    }
}


// ═══════════════════════════════════════════════════════
// INTERACTABLE HELPERS — Clues, Statues, Tablets, etc.
// ═══════════════════════════════════════════════════════

void MainWindow::addClueFragment(qreal x, qreal y)
{
    // Glow effect behind the clue
    auto *glow = scene->addRect(x - 6, y - 6, 40, 40,
                                 Qt::NoPen, QBrush(QColor(255, 210, 80, 40)));
    glow->setZValue(4);
    // NOT added to roomItems - tracked in clueFragments vector

    // Clue sprite
    auto *sprite = new QGraphicsPixmapItem(createClueSprite());
    sprite->setPos(x, y);
    sprite->setZValue(5);
    scene->addItem(sprite);

    clueFragments.append({sprite, glow, false, 0.f});
}

void MainWindow::addStatue(qreal x, qreal y, const QString &lore)
{
    auto *sprite = new QGraphicsPixmapItem(createStatueSprite());
    sprite->setPos(x, y);
    sprite->setZValue(3);
    scene->addItem(sprite);

    auto *hitbox = scene->addRect(x, y, 40, 80, Qt::NoPen, Qt::NoBrush);
    hitbox->setZValue(1);
    // NOT added to roomItems - tracked in statues vector

    statues.append({sprite, hitbox, lore});
}

void MainWindow::addTablet(qreal x, qreal y, const QString &hint)
{
    auto *sprite = new QGraphicsPixmapItem(createTabletSprite());
    sprite->setPos(x, y);
    sprite->setZValue(3);
    scene->addItem(sprite);

    auto *hitbox = scene->addRect(x, y, 50, 40, Qt::NoPen, Qt::NoBrush);
    hitbox->setZValue(1);
    // NOT added to roomItems - tracked in tablets vector

    tablets.append({sprite, hitbox, hint});
}

void MainWindow::addTorch(qreal x, qreal y, QList<QGraphicsItem*> reveals)
{
    auto *sprite = new QGraphicsPixmapItem(createTorchSprite(20, 45, false));
    sprite->setPos(x, y);
    sprite->setZValue(4);
    scene->addItem(sprite);

    auto *hitbox = scene->addRect(x - 5, y, 30, 50, Qt::NoPen, Qt::NoBrush);
    hitbox->setZValue(1);
    // NOT added to roomItems - tracked in torches vector

    torches.append({sprite, hitbox, false, reveals});
}

void MainWindow::addRelic(qreal x, qreal y, bool startVisible)
{
    auto *sprite = new QGraphicsPixmapItem(createRelicSprite());
    sprite->setPos(x, y);
    sprite->setZValue(5);
    sprite->setVisible(startVisible);
    scene->addItem(sprite);

    relics.append({sprite, false, startVisible});
}

void MainWindow::addAltarToRoom(qreal x, qreal y)
{
    auto *sprite = new QGraphicsPixmapItem(createAltarSprite(80, 60, false));
    sprite->setPos(x, y);
    sprite->setZValue(3);
    scene->addItem(sprite);

    auto *hitbox = scene->addRect(x, y, 80, 60, Qt::NoPen, Qt::NoBrush);
    hitbox->setZValue(1);

    auto *glowRect = scene->addRect(x - 10, y - 20, 100, 100,
                                     Qt::NoPen, QBrush(QColor(255, 230, 100, 0)));
    glowRect->setZValue(2);

    auto *hint = scene->addText("Press E");
    hint->setDefaultTextColor(QColor(255, 255, 120));
    QFont hf; hf.setPixelSize(13); hf.setBold(true);
    hint->setFont(hf);
    hint->setPos(x + 10, y - 22);
    hint->setZValue(20);
    hint->setVisible(false);

    // NOT added to roomItems - tracked in altar struct
    altar = {sprite, hitbox, glowRect, hint, false};
}

void MainWindow::tryCollectClue()
{
    if (!playerSprite) return;
    QPointF pc = playerSprite->sceneBoundingRect().center();

    for (auto &cf : clueFragments) {
        if (cf.collected || !cf.sprite) continue;

        QPointF cc = cf.sprite->sceneBoundingRect().center();
        float dist = qSqrt(qPow(pc.x() - cc.x(), 2) + qPow(pc.y() - cc.y(), 2));

        if (dist < 70.f) {
            cf.collected = true;
            cf.sprite->setVisible(false);
            if (cf.glowEffect) cf.glowEffect->setVisible(false);

            cluesFound++;
            setEventText(QString("Clue fragment collected! (%1/%2)")
                         .arg(cluesFound).arg(requiredClues));

            checkAltarActivation();
            return;
        }
    }
}

void MainWindow::tryInteractStatue()
{
    if (!playerSprite) return;
    QPointF pc = playerSprite->sceneBoundingRect().center();

    for (auto &s : statues) {
        if (!s.hitbox) continue;

        QPointF sc = s.hitbox->sceneBoundingRect().center();
        float dist = qSqrt(qPow(pc.x() - sc.x(), 2) + qPow(pc.y() - sc.y(), 2));

        if (dist < 70.f) {
            setEventText(s.loreText);
            return;
        }
    }
}

void MainWindow::tryInteractTablet()
{
    if (!playerSprite) return;
    QPointF pc = playerSprite->sceneBoundingRect().center();

    for (auto &tb : tablets) {
        if (!tb.hitbox) continue;

        QPointF tc = tb.hitbox->sceneBoundingRect().center();
        float dist = qSqrt(qPow(pc.x() - tc.x(), 2) + qPow(pc.y() - tc.y(), 2));

        if (dist < 70.f) {
            setEventText(tb.hintText);
            return;
        }
    }
}

void MainWindow::tryInteractTorch()
{
    if (!playerSprite) return;
    QPointF pc = playerSprite->sceneBoundingRect().center();

    for (auto &tr : torches) {
        if (!tr.hitbox) continue;

        QPointF trc = tr.hitbox->sceneBoundingRect().center();
        float dist = qSqrt(qPow(pc.x() - trc.x(), 2) + qPow(pc.y() - trc.y(), 2));

        if (dist < 70.f) {
            tr.activated = !tr.activated;
            tr.sprite->setPixmap(createTorchSprite(20, 45, tr.activated));

            if (tr.activated) {
                setEventText("The torch ignites! Something stirs in the shadows...");
                // Reveal hidden items
                for (auto *item : tr.revealsItems) {
                    item->setVisible(true);
                }
                // Also reveal any hidden relics
                for (auto &r : relics) {
                    if (!r.visible && r.sprite) {
                        r.visible = true;
                        r.sprite->setVisible(true);
                    }
                }
            } else {
                setEventText("The torch dims...");
                for (auto *item : tr.revealsItems) {
                    item->setVisible(false);
                }
            }
            return;
        }
    }
}

void MainWindow::tryCollectRelic()
{
    if (!playerSprite) return;
    QPointF pc = playerSprite->sceneBoundingRect().center();

    for (auto &r : relics) {
        if (r.collected || !r.visible || !r.sprite) continue;

        QPointF rc = r.sprite->sceneBoundingRect().center();
        float dist = qSqrt(qPow(pc.x() - rc.x(), 2) + qPow(pc.y() - rc.y(), 2));

        if (dist < 70.f) {
            r.collected = true;
            r.sprite->setVisible(false);

            relicsCollected++;
            score += 50;
            ui->scoreLabel->setText(QString("🏆 %1").arg(score));
            setEventText(QString("Ancient relic found! (+50 points) [%1/%2 relics]")
                         .arg(relicsCollected).arg(totalRelicsInGame));
            return;
        }
    }
}

void MainWindow::checkAltarActivation()
{
    if (!altar.sprite || altar.active) return;

    if (cluesFound >= requiredClues) {
        altar.active = true;
        altar.sprite->setPixmap(createAltarSprite(80, 60, true));

        // Animate glow
        if (altar.glowEffect) {
            altar.glowEffect->setBrush(QBrush(QColor(255, 230, 100, 80)));
        }

        setEventText("The altar awakens! Approach and press E to face the riddle.");
    }
}

void MainWindow::updateClueGlow()
{
    for (auto &cf : clueFragments) {
        if (cf.collected || !cf.glowEffect) continue;

        cf.glowPhase += 0.08f;
        int alpha = int(40 + 30 * qSin(cf.glowPhase));
        cf.glowEffect->setBrush(QBrush(QColor(255, 210, 80, alpha)));
    }

    // Also pulse altar glow if active
    if (altar.sprite && altar.active && altar.glowEffect) {
        static float altarPhase = 0;
        altarPhase += 0.06f;
        int alpha = int(60 + 40 * qSin(altarPhase));
        altar.glowEffect->setBrush(QBrush(QColor(255, 230, 100, alpha)));
    }
}

void MainWindow::updateEnemyChase()
{
    // This logic is now integrated into the main trap movement loop in onGameLoop()
}


// ═══════════════════════════════════════════════════════
// ROOM MECHANIC HELPERS — Spikes, Fake Doors, Collapsing
// ═══════════════════════════════════════════════════════

void MainWindow::addFakeDoor(qreal x, qreal y)
{
    auto *door = scene->addRect(x, y, 65, 100,
                                 QPen(QColor(255, 100, 50), 3),
                                 QBrush(QColor(140, 80, 20, 200)));
    door->setZValue(3);
    fakeDoors.append(door);
    // NOT added to roomItems - tracked in fakeDoors vector

    // Fake "ENTER" text
    auto *txt = addRoomText("ENTER", x + 8, y + 72, QColor(255, 180, 80), 13, 4);
    Q_UNUSED(txt);
}

void MainWindow::addSpikeTrap(qreal x, qreal y, qreal w, qreal h)
{
    auto *trap = scene->addRect(x, y, w, h,
                                 QPen(QColor(80, 60, 40), 1),
                                 QBrush(QColor(60, 45, 30, 180)));
    trap->setZValue(2);
    spikeTraps.append(trap);
    spikeActive.append(false);
    // NOT added to roomItems - tracked in spikeTraps vector
}

void MainWindow::addCollapsingTile(qreal x, qreal y, qreal w, qreal h)
{
    auto *tile = scene->addRect(x, y, w, h,
                                 QPen(QColor(100, 85, 55), 2),
                                 QBrush(QColor(85, 70, 45, 230)));
    tile->setZValue(2);
    collapsingTiles.append(tile);
    collapseTriggerTimer.append(-1);  // -1 = not triggered
    tileCollapsed.append(false);
    // NOT added to roomItems - tracked in collapsingTiles vector
}

void MainWindow::updateSpikeTraps()
{
    spikeTimer++;

    // Spikes cycle every 120 frames (2 seconds): 60 frames up, 60 frames down
    bool spikesUp = (spikeTimer % 120) < 60;

    for (int i = 0; i < spikeTraps.size(); i++) {
        // Alternate pattern: even/odd spikes offset
        bool thisUp = (i % 2 == 0) ? spikesUp : !spikesUp;
        spikeActive[i] = thisUp;

        QColor col = thisUp ? QColor(180, 50, 50, 220) : QColor(60, 45, 30, 180);
        spikeTraps[i]->setBrush(QBrush(col));

        // Check player collision with active spikes
        if (thisUp && playerSprite && !invincible) {
            if (playerSprite->sceneBoundingRect().intersects(
                    spikeTraps[i]->sceneBoundingRect())) {
                onTrapHit();
            }
        }
    }
}

void MainWindow::updateCollapsingFloor()
{
    if (!playerSprite) return;

    QRectF pr = playerSprite->sceneBoundingRect();

    for (int i = 0; i < collapsingTiles.size(); i++) {
        QRectF tr = collapsingTiles[i]->sceneBoundingRect();

        // If tile already collapsed, damage player if they walk on it
        if (tileCollapsed[i]) {
            if (pr.intersects(tr) && !invincible) {
                onTrapHit();
                setEventText("You fall through the broken floor!");
            }
            continue;
        }

        // Check if player is standing on tile
        if (pr.intersects(tr)) {
            if (collapseTriggerTimer[i] < 0) {
                // Start collapse countdown (90 frames = 1.5 seconds)
                collapseTriggerTimer[i] = 90;
                collapsingTiles[i]->setBrush(QBrush(QColor(140, 80, 40, 230)));
            }
        }

        // Count down and collapse
        if (collapseTriggerTimer[i] > 0) {
            collapseTriggerTimer[i]--;

            // Visual warning: shake/flash
            int shake = (collapseTriggerTimer[i] % 6 < 3) ? 1 : -1;
            QRectF r = collapsingTiles[i]->rect();
            collapsingTiles[i]->setPos(shake, 0);

            if (collapseTriggerTimer[i] <= 0) {
                // Collapse!
                tileCollapsed[i] = true;
                collapsingTiles[i]->setBrush(QBrush(QColor(20, 15, 10, 100)));
                collapsingTiles[i]->setPen(QPen(QColor(40, 30, 20, 100)));

                // Check if player falls
                if (pr.intersects(tr) && !invincible) {
                    onTrapHit();
                    setEventText("The floor gives way beneath you!");
                }
            }
        }
    }
}

void MainWindow::checkFakeDoorCollision()
{
    if (!playerSprite || invincible) return;

    QRectF pr = playerSprite->sceneBoundingRect();

    for (auto *fd : fakeDoors) {
        if (pr.intersects(fd->sceneBoundingRect())) {
            onTrapHit();
            setEventText("A trap! That door was a deadly illusion!");
            return;
        }
    }
}


// ═══════════════════════════════════════════════════════
// ROOM INTRO — Story popup when entering each level
// ═══════════════════════════════════════════════════════

void MainWindow::showRoomIntro(int room)
{
    static const QString ROOM_STORIES[7] = {
        // Room 0: Entrance Chamber
        "ENTRANCE CHAMBER\n\n"
        "You descend into the ancient tomb, torchlight flickering against "
        "hieroglyph-covered walls. The air is thick with centuries of dust.\n\n"
        "Your mission: Find the scattered clue fragments hidden throughout "
        "each chamber. Once you collect enough, the central altar will "
        "awaken, presenting you with an ancient riddle.\n\n"
        "Explore carefully. The tomb holds many secrets.",

        // Room 1: Guardian Hall
        "GUARDIAN HALL\n\n"
        "A massive stone guardian patrols these halls — the eternal protector "
        "of the pharaoh's secrets. Its glowing eyes scan for intruders.\n\n"
        "WARNING: The guardian can detect movement from great distances. "
        "Use the stone pillars for cover. Move only when its back is turned. "
        "If spotted, it will pursue relentlessly.\n\n"
        "Stealth is your ally here.",

        // Room 2: Hall of Echoes
        "HALL OF ECHOES\n\n"
        "Ghostly spectres drift through this ethereal chamber, remnants of "
        "souls who failed the tomb's trials. Strange illusions shimmer in "
        "the purple mist.\n\n"
        "BEWARE: Not all doors are what they seem. Some are deadly traps "
        "designed to punish the hasty. The spectres patrol their eternal "
        "routes but will not give chase.\n\n"
        "Trust your instincts. Find the true path.",

        // Room 3: Trial Room
        "TRIAL ROOM\n\n"
        "Ancient mechanisms line the floor — pressure plates connected to "
        "deadly spike traps that rise and fall in an eternal rhythm.\n\n"
        "The spikes alternate in a pattern. Watch carefully, time your "
        "movements, and cross during the safe moments. The mummy guardians "
        "patrol but will not pursue you.\n\n"
        "Patience will see you through.",

        // Room 4: Ankh Sanctuary
        "ANKH SANCTUARY\n\n"
        "Golden light bathes this sacred chamber, home to the Ankh — "
        "symbol of eternal life. Scarab beetles patrol the sanctuary, "
        "sacred guardians of the ancient artifact.\n\n"
        "A hidden relic lies somewhere in the darkness. Light the torch "
        "to reveal what shadows conceal. The scarabs keep to their paths "
        "and will not give chase.\n\n"
        "The Ankh awaits the worthy.",

        // Room 5: Khonshu's Chamber
        "KHONSHU'S CHAMBER\n\n"
        "The Moon God Khonshu rises before you, his crescent staff "
        "glowing with divine power. This is no puzzle — this is battle.\n\n"
        "Khonshu hurls crescent projectiles at you. Dodge behind the "
        "stone covers, but beware — they can only absorb 5 hits before "
        "crumbling. When Khonshu tires and glows, rush in and strike "
        "with R. You can land up to 5 hits per opening.\n\n"
        "Defeat the Moon God to reach the final gate.",

        // Room 6: Anubis Gate
        "ANUBIS GATE\n\n"
        "The final chamber. Anubis, Judge of the Dead, awaits at the "
        "gate between worlds. The floor itself is cursed — ancient tiles "
        "crumble moments after you step on them.\n\n"
        "Dark mummies patrol these halls. Move quickly but carefully. "
        "Once a tile falls, there is no return. Find the clues, face "
        "the final riddle, and escape the tomb.\n\n"
        "Your journey ends here — one way or another."
    };

    if (room < 0 || room > 6) return;

    // Create story dialog
    QDialog *intro = new QDialog(this);
    intro->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    intro->setModal(true);
    intro->setFixedSize(500, 380);
    intro->setStyleSheet(
        "QDialog {"
        "  background-color: #0d0905;"
        "  border: 3px solid #c8a03c;"
        "  border-radius: 16px;"
        "}"
    );

    QVBoxLayout *layout = new QVBoxLayout(intro);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(15);

    // Story text
    QLabel *storyLabel = new QLabel(ROOM_STORIES[room], intro);
    storyLabel->setWordWrap(true);
    storyLabel->setStyleSheet(
        "color: #e8d8b0; font-size: 13px; font-family: Copperplate;"
        "background: transparent; line-height: 1.4;"
    );
    storyLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    layout->addWidget(storyLabel);

    layout->addStretch();

    // Continue button
    QPushButton *continueBtn = new QPushButton("Enter Chamber", intro);
    continueBtn->setStyleSheet(
        "QPushButton { background: rgba(200,160,60,200); color: #0d0905;"
        "border: 2px solid #ffe090; border-radius: 8px; font-size: 14px;"
        "font-weight: bold; padding: 10px 30px; }"
        "QPushButton:hover { background: rgba(255,200,80,220); }"
    );
    connect(continueBtn, &QPushButton::clicked, intro, &QDialog::accept);
    layout->addWidget(continueBtn, 0, Qt::AlignCenter);

    intro->exec();
    delete intro;
}
