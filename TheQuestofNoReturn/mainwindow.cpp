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

// ─── Male explorer (Karim / Rami) ─────────────────────────────────────────────
static QPixmap createPlayerSpriteMale(int frame = 0)
{
    QPixmap pix(48, 72);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    // Palette
    const QColor helm  (212, 193, 132);  // pith helmet ivory
    const QColor helmD (162, 144,  90);  // helmet shadow
    const QColor gogR  ( 84,  58,  20);  // goggle rim
    const QColor gogL  (148, 198, 216);  // goggle lens
    const QColor hatBd ( 86,  48,  12);  // hat band
    const QColor brim  (190, 170, 110);  // brim
    const QColor skin  (222, 172, 102);  // face / hands
    const QColor skinD (180, 134,  72);  // skin shadow
    const QColor eyeW  (236, 230, 218);  // eye white
    const QColor eyeP  ( 26,  12,   2);  // pupil
    const QColor brow  ( 70,  44,  14);  // eyebrow
    const QColor stub  (138,  96,  56);  // stubble
    const QColor lip   (142,  78,  55);  // lips
    const QColor shirt (150, 140, 102);  // khaki shirt
    const QColor shrtD (115, 106,  74);  // shirt shadow
    const QColor strap ( 68,  40,  10);  // bandolier leather
    const QColor pouch ( 90,  58,  20);  // ammo pouch
    const QColor belt  ( 58,  34,  10);  // belt
    const QColor buckl (176, 146,  44);  // buckle
    const QColor pants (122, 112,  72);  // khaki pants
    const QColor pantsD( 90,  82,  50);  // pants shadow
    const QColor pckt  (100,  92,  56);  // cargo pocket
    const QColor boot  ( 50,  26,   7);  // leather boot
    const QColor bootD ( 30,  14,   3);  // boot sole

    // ── Pith helmet dome ───────────────────────────────────────────────────────
    p.fillRect(19, 0, 10,  1, helm);
    p.fillRect(16, 1, 16,  1, helm);
    p.fillRect(13, 2, 22,  1, helm);
    p.fillRect(12, 3, 24,  5, helm);
    p.fillRect(29, 3,  4,  5, helmD);   // dome right shadow
    p.fillRect(33, 5,  2,  3, helmD);

    // ── Goggles pushed up on dome ──────────────────────────────────────────────
    p.fillRect(14,  4,  5,  4, gogR);   // left frame
    p.fillRect(15,  5,  3,  2, gogL);   // left lens
    p.fillRect(19,  4,  2,  4, gogR);   // bridge
    p.fillRect(21,  4,  5,  4, gogR);   // right frame
    p.fillRect(22,  5,  3,  2, gogL);   // right lens

    // ── Hat band ───────────────────────────────────────────────────────────────
    p.fillRect(12,  8, 24,  2, hatBd);

    // ── Brim ───────────────────────────────────────────────────────────────────
    p.fillRect( 3, 10, 42,  2, brim);
    p.fillRect( 5, 12, 38,  1, helmD);  // underside shadow

    // ── Face ───────────────────────────────────────────────────────────────────
    p.fillRect(13, 13, 22, 13, skin);
    p.fillRect(13, 21,  2,  5, skinD);  // jaw shadow L
    p.fillRect(33, 21,  2,  5, skinD);  // jaw shadow R
    p.fillRect(15, 25, 18,  1, skin);   // chin bottom

    // Eyebrows (slightly furrowed — determined look)
    p.fillRect(16, 15,  6,  1, brow);
    p.fillRect(26, 15,  6,  1, brow);
    p.fillRect(16, 14,  2,  1, brow);   // inner raised brow
    p.fillRect(30, 14,  2,  1, brow);

    // Eyes
    p.fillRect(16, 17,  6,  3, eyeW);   // left white
    p.fillRect(18, 17,  3,  3, eyeP);   // left pupil
    p.fillRect(16, 16,  6,  1, brow);   // left lid
    p.fillRect(26, 17,  6,  3, eyeW);   // right white
    p.fillRect(28, 17,  3,  3, eyeP);   // right pupil
    p.fillRect(26, 16,  6,  1, brow);   // right lid

    // Nose
    p.fillRect(22, 20,  4,  1, skinD);
    p.fillRect(21, 21,  2,  1, skinD);
    p.fillRect(25, 21,  2,  1, skinD);

    // Mouth — slight determined smirk
    p.fillRect(19, 23,  9,  1, lip);
    p.fillRect(19, 24,  3,  1, lip);
    p.fillRect(27, 24,  1,  1, lip);

    // Stubble (scattered 1px dots on lower jaw)
    for (int sx : {14,16,19,22,26,29,32}) p.fillRect(sx, 25, 1, 1, stub);
    for (int sx : {15,18,21,24,27,31})    p.fillRect(sx, 26, 1, 1, stub);

    // ── Neck ───────────────────────────────────────────────────────────────────
    p.fillRect(19, 27,  9,  3, skin);

    // ── Shirt / torso ──────────────────────────────────────────────────────────
    p.fillRect(11, 30, 26, 21, shirt);
    p.fillRect(11, 30,  3, 21, shrtD);  // left edge shadow
    p.fillRect(34, 30,  3, 21, shrtD);  // right edge shadow

    // V-neck collar & exposed chest
    p.fillRect(19, 30, 10,  2, skin);
    p.fillRect(20, 32,  8,  5, skin);
    p.fillRect(19, 32,  2,  4, shirt);  // left lapel
    p.fillRect(27, 32,  2,  4, shirt);  // right lapel

    // Left breast pocket with flap
    p.fillRect(13, 34,  7,  7, shrtD);
    p.fillRect(14, 35,  5,  5, shirt);
    p.fillRect(14, 34,  5,  1, shrtD);  // pocket flap
    p.fillRect(16, 34,  1,  7, shrtD);  // pocket centre line

    // ── Bandolier strap (diagonal L-shoulder → R-hip) ─────────────────────────
    p.fillRect(11, 30,  2,  2, strap);
    p.fillRect(13, 32,  2,  2, strap);
    p.fillRect(15, 34,  3,  3, pouch);  // pouch 1
    p.fillRect(17, 36,  2,  2, strap);
    p.fillRect(19, 38,  2,  2, strap);
    p.fillRect(21, 40,  3,  3, pouch);  // pouch 2
    p.fillRect(23, 42,  2,  2, strap);
    p.fillRect(25, 44,  2,  2, strap);
    p.fillRect(27, 46,  3,  3, pouch);  // pouch 3

    // ── Belt ───────────────────────────────────────────────────────────────────
    p.fillRect(11, 51, 26,  3, belt);
    p.fillRect(20, 50,  8,  5, buckl);  // buckle plate
    p.fillRect(22, 52,  4,  1, belt);   // buckle pin

    // ── Arms ───────────────────────────────────────────────────────────────────
    // Left arm (rolled sleeve — forearm visible)
    p.fillRect( 3, 30,  8, 18, shirt);
    p.fillRect( 3, 48,  8,  1, shrtD);  // sleeve roll cuff
    p.fillRect( 3, 49,  8,  7, skin);   // forearm
    p.fillRect( 4, 56,  6,  3, skin);   // hand

    // Right arm
    p.fillRect(37, 30,  8, 18, shirt);
    p.fillRect(37, 48,  8,  1, shrtD);
    p.fillRect(37, 49,  8,  7, skin);
    p.fillRect(38, 56,  6,  3, skin);

    // ── Legs ───────────────────────────────────────────────────────────────────
    int lOff = (frame == 1) ? -3 : 0;
    int rOff = (frame == 1) ?  3 : 0;

    // Left leg
    p.fillRect(11 + lOff, 54, 11, 13, pants);
    p.fillRect(11 + lOff, 54,  2, 13, pantsD);  // inner shadow
    // Left cargo pocket
    p.fillRect(12 + lOff, 60,  7,  5, pckt);
    p.fillRect(13 + lOff, 61,  5,  4, pants);
    p.fillRect(13 + lOff, 60,  5,  1, pckt);

    // Right leg
    p.fillRect(26 + rOff, 54, 11, 13, pants);
    p.fillRect(34 + rOff, 54,  2, 13, pantsD);  // outer shadow
    // Right cargo pocket
    p.fillRect(27 + rOff, 60,  7,  5, pckt);
    p.fillRect(28 + rOff, 61,  5,  4, pants);
    p.fillRect(28 + rOff, 60,  5,  1, pckt);

    // ── Boots ──────────────────────────────────────────────────────────────────
    p.fillRect( 8 + lOff, 67, 14,  4, boot);        // left boot
    p.fillRect(10 + lOff, 68,  4,  1, QColor(80,46,16)); // left highlight
    p.fillRect( 7 + lOff, 71, 16,  1, bootD);        // left sole

    p.fillRect(26 + rOff, 67, 14,  4, boot);         // right boot
    p.fillRect(28 + rOff, 68,  4,  1, QColor(80,46,16));
    p.fillRect(25 + rOff, 71, 16,  1, bootD);         // right sole

    p.end();
    return pix;
}

// ─── Female explorer — Lara Croft style (Zara / Nour) ────────────────────────
static QPixmap createPlayerSpriteFemale(int frame = 0)
{
    QPixmap pix(48, 72);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    // Palette
    const QColor hair  ( 50,  24,   4);   // dark brown
    const QColor hairH ( 86,  46,  14);   // hair sheen
    const QColor skin  (218, 168,  96);   // face / arms / legs
    const QColor skinD (172, 126,  64);   // skin shadow
    const QColor eyeW  (234, 228, 216);
    const QColor eyeP  ( 28,  14,   4);
    const QColor brow  ( 65,  40,  10);
    const QColor lip   (186,  80,  64);   // rose-red
    const QColor lipHi (210, 118,  96);
    const QColor top   ( 52, 148, 138);   // iconic teal tank top
    const QColor topD  ( 32, 104,  96);   // top shadow
    const QColor topH  ( 78, 178, 168);   // top highlight
    const QColor belt  ( 68,  40,  12);
    const QColor buckl (176, 144,  42);
    const QColor pouch ( 86,  54,  18);
    const QColor shts  ( 94,  58,  20);   // warm brown shorts
    const QColor shtsD ( 66,  38,  10);
    const QColor hlstr ( 56,  30,   6);   // thigh holster
    const QColor gun   ( 46,  46,  50);   // pistol
    const QColor hstrp ( 44,  24,   4);   // holster strap
    const QColor boot  ( 78,  46,  14);   // light brown boot
    const QColor bootD ( 50,  26,   6);   // boot shadow
    const QColor sole  ( 28,  12,   2);
    const QColor lace  (188, 156,  98);

    // ── Hair top / bun ─────────────────────────────────────────────────────────
    p.fillRect(16,  0, 16,  1, hair);
    p.fillRect(14,  1, 20,  2, hair);
    p.fillRect(14,  0,  4,  1, hairH);
    p.fillRect(30,  0,  9,  5, hair);    // bun crown-right
    p.fillRect(31,  1,  6,  3, hairH);

    // ── Face ──────────────────────────────────────────────────────────────────
    p.fillRect(14,  2, 20, 15, skin);
    p.fillRect(16, 16, 16,  1, skin);    // chin

    // Eyebrows — thin, softly arched
    p.fillRect(17,  5,  5,  1, brow);
    p.fillRect(26,  5,  5,  1, brow);

    // Eyes — 5×2 with lash line and outer flick
    p.fillRect(17,  7,  5,  2, eyeW);   // left white
    p.fillRect(18,  7,  3,  2, eyeP);   // left pupil
    p.fillRect(17,  6,  5,  1, brow);   // left lash
    p.fillRect(16,  6,  1,  1, brow);   // left flick
    p.fillRect(25,  7,  5,  2, eyeW);   // right white
    p.fillRect(26,  7,  3,  2, eyeP);   // right pupil
    p.fillRect(25,  6,  5,  1, brow);   // right lash
    p.fillRect(30,  6,  1,  1, brow);   // right flick

    // Subtle cheek blush
    p.fillRect(15,  9,  2,  2, QColor(228, 148, 112));
    p.fillRect(31,  9,  2,  2, QColor(228, 148, 112));

    // Nose — minimal, 3 dots
    p.fillRect(22, 10,  3,  1, skinD);
    p.fillRect(21, 11,  2,  1, skinD);
    p.fillRect(24, 11,  2,  1, skinD);

    // Lips — 7px, cupid's bow
    p.fillRect(20, 13,  7,  1, lip);
    p.fillRect(20, 14,  7,  1, lip);
    p.fillRect(22, 13,  3,  1, lipHi);  // bow highlight

    // ── Side hair — 4px, gently frames face (drawn after features) ────────────
    p.fillRect(11,  2,  4, 15, hair);   // left (2px overlap into face)
    p.fillRect(33,  2,  4, 15, hair);   // right (2px overlap into face)
    p.fillRect(11,  2,  2,  9, hairH);  // left sheen
    p.fillRect(35,  2,  2,  9, hairH);  // right sheen
    p.fillRect(12, 14,  3,  3, hair);   // left jaw wisp
    p.fillRect(33, 14,  3,  3, hair);   // right jaw wisp

    // ── Neck ───────────────────────────────────────────────────────────────────
    p.fillRect(19, 17,  8,  3, skin);

    // ── Teal tank top ──────────────────────────────────────────────────────────
    p.fillRect(13, 20, 22, 19, top);
    p.fillRect(13, 20,  3, 19, topD);   // left shadow
    p.fillRect(32, 20,  3, 19, topD);   // right shadow
    p.fillRect(14, 20, 20,  2, topH);   // collar shine
    // Tank straps (narrow, over bare shoulders)
    p.fillRect(18, 17,  3,  4, top);
    p.fillRect(27, 17,  3,  4, top);

    // ── Bare arms — athletic, skin-coloured ────────────────────────────────────
    // Left
    p.fillRect( 4, 20,  9, 13, skin);
    p.fillRect( 4, 33,  9,  9, skin);
    p.fillRect( 5, 42,  7,  3, skin);
    p.fillRect( 4, 20,  2, 13, skinD);  // arm shadow
    // Right
    p.fillRect(35, 20,  9, 13, skin);
    p.fillRect(35, 33,  9,  9, skin);
    p.fillRect(36, 42,  7,  3, skin);
    p.fillRect(42, 20,  2, 13, skinD);

    // ── Belt + hip pouches ────────────────────────────────────────────────────
    p.fillRect(13, 39, 22,  3, belt);
    p.fillRect(20, 38,  8,  5, buckl);
    p.fillRect(22, 40,  4,  1, belt);
    p.fillRect(13, 39,  4,  5, pouch);   // left hip pouch
    p.fillRect(31, 39,  4,  5, pouch);   // right hip pouch

    // ── Brown shorts ──────────────────────────────────────────────────────────
    p.fillRect(13, 42, 22,  9, shts);
    p.fillRect(13, 42,  3,  9, shtsD);
    p.fillRect(32, 42,  3,  9, shtsD);
    p.fillRect(22, 42,  4,  9, shtsD);   // centre inseam

    // ── Bare legs ─────────────────────────────────────────────────────────────
    int lOff = (frame == 1) ? -3 : 0;
    int rOff = (frame == 1) ?  3 : 0;

    p.fillRect(13 + lOff, 51,  9, 11, skin);
    p.fillRect(13 + lOff, 51,  2, 11, skinD);
    p.fillRect(26 + rOff, 51,  9, 11, skin);
    p.fillRect(33 + rOff, 51,  2, 11, skinD);

    // ── Thigh holsters (outer thighs, with leg strap) ─────────────────────────
    // Left holster
    p.fillRect( 7 + lOff, 46,  2, 14, hstrp);   // vertical leg strap
    p.fillRect( 7 + lOff, 50,  7,  1, hstrp);   // horizontal strap
    p.fillRect( 7 + lOff, 51,  6,  8, hlstr);   // holster body
    p.fillRect( 8 + lOff, 52,  4,  5, gun);      // pistol in holster
    p.fillRect( 8 + lOff, 52,  1,  5, QColor(65,65,70)); // slide detail

    // Right holster
    p.fillRect(34 + rOff, 46,  2, 14, hstrp);
    p.fillRect(34 + rOff, 50,  7,  1, hstrp);
    p.fillRect(35 + rOff, 51,  6,  8, hlstr);
    p.fillRect(36 + rOff, 52,  4,  5, gun);
    p.fillRect(39 + rOff, 52,  1,  5, QColor(65,65,70));

    // ── Knee-high lace-up boots ───────────────────────────────────────────────
    // Left boot
    p.fillRect( 9 + lOff, 62, 13,  8, boot);
    p.fillRect( 9 + lOff, 62,  2,  8, bootD);   // shadow
    p.fillRect(12 + lOff, 63,  5,  1, lace);    // lace 1
    p.fillRect(11 + lOff, 65,  6,  1, lace);    // lace 2
    p.fillRect(12 + lOff, 67,  5,  1, lace);    // lace 3
    p.fillRect( 8 + lOff, 70, 15,  1, sole);

    // Right boot
    p.fillRect(26 + rOff, 62, 13,  8, boot);
    p.fillRect(35 + rOff, 62,  2,  8, bootD);
    p.fillRect(27 + rOff, 63,  5,  1, lace);
    p.fillRect(27 + rOff, 65,  6,  1, lace);
    p.fillRect(27 + rOff, 67,  5,  1, lace);
    p.fillRect(25 + rOff, 70, 15,  1, sole);

    // ── Braid — drawn last so it sits over right shoulder ─────────────────────
    // Upper braid (over shoulder, x=34-38 so it's beside/over the right arm)
    p.fillRect(34,  4,  4,  6, hair);    // from bun down neck-right
    p.fillRect(35, 10,  4,  8, hair);    // over right shoulder
    p.fillRect(35, 10,  2,  5, hairH);   // sheen
    // Braid body (hanging down right side of torso, in front of arm)
    p.fillRect(36, 18,  4, 10, hair);
    p.fillRect(36, 28,  3, 10, hair);
    p.fillRect(37, 38,  3,  8, hair);    // braid tip
    p.fillRect(36, 19,  2,  5, hairH);

    p.end();
    return pix;
}

// ─── Dispatcher: picks male or female by character ────────────────────────────
static QPixmap createPlayerSprite(int frame, bool female)
{
    return female ? createPlayerSpriteFemale(frame) : createPlayerSpriteMale(frame);
}

// ─── Khonshu boss sprite ──────────────────────────────────────────────────────
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

    p.setBrush(crownCol); p.setPen(Qt::NoPen);
    p.drawEllipse(w/2 - 18, 0, 36, 26);
    p.setBrush(bgCut);
    p.drawEllipse(w/2 - 6, 0, 36, 26);

    p.setBrush(crownCol); p.setPen(QPen(QColor(255,230,100), 1));
    p.drawEllipse(w/2 - 11, 20, 22, 14);

    int hdY = 30, hdW = 50, hdH = 38, hdX = (w - hdW) / 2;
    p.setBrush(headCol); p.setPen(Qt::NoPen);
    p.fillRect(hdX, hdY, hdW, hdH, headCol);
    p.fillRect(hdX + 4, hdY - 4, hdW - 8, 6, headCol);
    p.fillRect(hdX + hdW * 2/3, hdY + hdH/3, 7, 5, eyeCol);
    p.setBrush(beakCol);
    QPolygonF beak;
    beak << QPointF(hdX + hdW,      hdY + hdH * 0.40)
         << QPointF(hdX + hdW + 14, hdY + hdH * 0.52)
         << QPointF(hdX + hdW,      hdY + hdH * 0.68);
    p.drawPolygon(beak);

    int colY = hdY + hdH, colH = 18, colW = 62, colX = (w - colW) / 2;
    p.fillRect(colX, colY, colW, colH, collarBl);
    for (int i = 0; i < 4; i++)
        p.fillRect(colX, colY + i * 4 + 1, colW, 2, jewelCol);

    int bdY = colY + colH, bdW = 58, bdH = 94, bdX = (w - bdW) / 2;
    p.fillRect(bdX, bdY, bdW, bdH, robeCol);
    for (int i = 1; i <= 7; i++)
        p.fillRect(bdX, bdY + bdH * i / 8, bdW, 2, wrapCol);
    p.fillRect(w/2 - 1, bdY, 2, bdH, wrapCol);
    p.fillRect(w/2 - 8, bdY + 4, 16, 11, jewelCol);

    p.fillRect(bdX - 13, bdY + 4, 13, 54, robeCol);
    p.fillRect(bdX - 13, bdY + 4,  13, 2, wrapCol);
    p.fillRect(bdX - 13, bdY + 28, 13, 2, wrapCol);
    p.fillRect(bdX + bdW, bdY + 4, 13, 38, robeCol);
    p.fillRect(bdX + bdW, bdY + 4,  13, 2, wrapCol);
    p.fillRect(bdX + bdW, bdY + 22, 13, 2, wrapCol);

    int stX = bdX + bdW + 13 + 3;
    p.fillRect(stX - 2, 2, 4, h - 2, staffCol);
    p.setBrush(staffCol); p.setPen(Qt::NoPen);
    p.drawEllipse(stX - 11, 2, 22, 18);
    p.setBrush(bgCut);
    p.drawEllipse(stX - 3, 2, 22, 18);

    int lgY = bdY + bdH, lgH = h - lgY;
    if (lgH > 0) {
        p.fillRect(bdX, lgY, bdW, lgH, robeCol);
        for (int i = 1; i < 4; i++)
            p.fillRect(bdX, lgY + i * lgH / 3, bdW, 2, wrapCol);
    }

    p.end();
    return pix;
}

// ─── Moon crescent projectile sprite ─────────────────────────────────────────
static QPixmap createCrescentSprite(int w, int h)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    p.setBrush(QColor(220, 192, 60));
    p.setPen(QPen(QColor(255, 232, 110), 1));
    p.drawEllipse(1, 1, w - 2, h - 2);

    p.setBrush(QColor(8, 4, 28, 255));
    p.setPen(Qt::NoPen);
    p.drawEllipse(w / 3, 1, w - 2, h - 2);

    p.end();
    return pix;
}

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
// CONSTRUCTOR
// ═══════════════════════════════════════════════════════

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_hud(nullptr)
    , countdownTimer(nullptr)
    , loopTimer(nullptr)
    , scene(nullptr)
    , playerSprite(nullptr)
    , doorItem(nullptr)
    , pressEHint(nullptr)
    , bossSprite(nullptr)
    , bossHPBar(nullptr)
    , realDoor(nullptr)
    , m_altar(nullptr)
{
    ui->setupUi(this);

    // ── Room-map sidebar labels (index matches room order) ─
    m_mapLabels[0] = ui->label_7;   // Entrance Chamber
    m_mapLabels[1] = ui->label_10;  // Guardian Hall
    m_mapLabels[2] = ui->label_12;  // Hall of Echoes
    m_mapLabels[3] = ui->label_11;  // Trial Room
    m_mapLabels[4] = ui->label_8;   // Ankh Sanctuary
    m_mapLabels[5] = ui->label_9;   // Khonshu / Anubis Gate

    // GameHUD is initialized after the map labels are set
    m_hud = new GameHUD(ui, m_mapLabels);

    // ── Timers ──────────────────────────────────────────
    countdownTimer = new QTimer(this);
    countdownTimer->setInterval(1000);
    connect(countdownTimer, &QTimer::timeout, this, &MainWindow::onCountdownTick);

    loopTimer = new QTimer(this);
    loopTimer->setInterval(16);   // ≈ 60 fps
    connect(loopTimer, &QTimer::timeout, this, &MainWindow::onGameLoop);

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
                m_player.character = "Zara";
                ui->portraitLabel->setPixmap(QPixmap(":/images/Zara.png"));
                ui->descriptionLabel->setText("Fearless explorer driven by curiosity and courage.");
            });

    connect(ui->karimButton, &QPushButton::clicked,
            this, [=]() {
                m_player.character = "Karim";
                ui->portraitLabel->setPixmap(QPixmap(":/images/Karim.png"));
                ui->descriptionLabel->setText("Balanced adventurer skilled in combat and survival.");
            });

    connect(ui->nourButton, &QPushButton::clicked,
            this, [=]() {
                m_player.character = "Nour";
                ui->portraitLabel->setPixmap(QPixmap(":/images/NOOR.png"));
                ui->descriptionLabel->setText("Strategic thinker who uncovers hidden ancient secrets.");
            });

    connect(ui->ramiButton, &QPushButton::clicked,
            this, [=]() {
                m_player.character = "Rami";
                ui->portraitLabel->setPixmap(QPixmap(":/images/RAMI.png"));
                ui->descriptionLabel->setText("Fast and daring explorer who trusts instinct.");
            });


    // ═══════════════════════════════════════════════════
    // TRAIT SELECTION
    // ═══════════════════════════════════════════════════

    connect(ui->scholarButton, &QPushButton::clicked,
            this, [=]() {
                m_player.trait = "Scholar";
                ui->traitDescriptionLabel->setText("Gain intelligence bonuses and uncover hidden clues.");
                ui->traitIconLabel->setPixmap(QPixmap(":/images/scholarIcon.png"));
            });

    connect(ui->survivorButton, &QPushButton::clicked,
            this, [=]() {
                m_player.trait = "Survivor";
                ui->traitDescriptionLabel->setText("Higher endurance and resistance to danger.");
                ui->traitIconLabel->setPixmap(QPixmap(":/images/survivorIcon.png"));
            });

    connect(ui->speedsterButton, &QPushButton::clicked,
            this, [=]() {
                m_player.trait = "Speedster";
                ui->traitDescriptionLabel->setText("Move faster and escape traps more easily.");
                ui->traitIconLabel->setPixmap(QPixmap(":/images/speedsterIcon.png"));
            });


    // ═══════════════════════════════════════════════════
    // CONFIRM CHARACTER BUTTON
    // ═══════════════════════════════════════════════════

    connect(ui->confirmButton, &QPushButton::clicked,
            this, [=]() {

                m_player.name = ui->lineEdit->text();

                if (m_player.name.isEmpty()) {
                    ui->traitDescriptionLabel->setText("Enter your explorer name first.");
                    return;
                }

                ui->hudNameLabel->setText(m_player.name.toUpper());
                ui->hudTraitLabel->setText(m_player.trait.toUpper());

                if (m_player.character == "Zara")
                    ui->hudPortraitLabel->setPixmap(QPixmap(":/images/Zara.png"));
                else if (m_player.character == "Karim")
                    ui->hudPortraitLabel->setPixmap(QPixmap(":/images/Karim.png"));
                else if (m_player.character == "Nour")
                    ui->hudPortraitLabel->setPixmap(QPixmap(":/images/NOOR.png"));
                else if (m_player.character == "Rami")
                    ui->hudPortraitLabel->setPixmap(QPixmap(":/images/RAMI.png"));

                ui->stackedWidget->setCurrentWidget(ui->storyPage);
            });


    // ═══════════════════════════════════════════════════
    // ENTER TOMB BUTTON
    // ═══════════════════════════════════════════════════

    connect(ui->enterTombButton, &QPushButton::clicked,
            this, [=]() {
                ui->stackedWidget->setCurrentWidget(ui->gamePage);
                startGame();
            });


    // ═══════════════════════════════════════════════════
    // PAUSE MENU BUTTON
    // ═══════════════════════════════════════════════════

    connect(ui->pauseTestButton, &QPushButton::clicked,
            this, [=]() {

                if (!m_state.gameActive) return;
                loopTimer->stop();
                countdownTimer->stop();
                heldKeys.clear();

                PauseMenuDialog dialog(this);

                connect(&dialog, &PauseMenuDialog::resumeClicked,
                        this, [=]() {
                            if (m_state.gameActive) {
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

                if (m_state.gameActive && !countdownTimer->isActive()) {
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
    // DIFFICULTY BUTTON
    // ═══════════════════════════════════════════════════

    connect(ui->difficultyButton, &QPushButton::clicked,
            this, [=]() {

                DifficultyDialog dialog(this);

                connect(&dialog, &DifficultyDialog::difficultySelected,
                        this, [=](Difficulty difficulty) {

                            m_player.difficulty = difficulty;

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
    delete m_hud;
    delete ui;
}


// ═══════════════════════════════════════════════════════
// KEY EVENTS
// ═══════════════════════════════════════════════════════

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    if (ui->stackedWidget->currentWidget() == ui->gamePage && m_state.gameActive) {
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
// COUNTDOWN TICK
// ═══════════════════════════════════════════════════════

void MainWindow::onCountdownTick()
{
    if (m_state.timeSeconds > 0) m_state.timeSeconds--;
    m_hud->refreshTimer(m_state.timeSeconds);
    if (m_state.timeSeconds == 0) {
        countdownTimer->stop();
        triggerGameOver("Time has run out. The tomb has claimed you.");
    }
}


// ═══════════════════════════════════════════════════════
// GAME LOOP  — fires every ~16 ms (≈60 fps)
// ═══════════════════════════════════════════════════════

void MainWindow::onGameLoop()
{
    if (!m_state.gameActive || !playerSprite) return;

    // ── Move player ────────────────────────────────────
    float spd = (m_player.trait == "Speedster") ? 7.f : 5.f;
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
    const bool female = (m_player.character == "Zara" || m_player.character == "Nour");
    bool isMoving = (dx != 0.f || dy != 0.f);
    if (isMoving) {
        if (++m_state.animTick >= 8) {
            m_state.animTick = 0;
            m_state.animFrame ^= 1;
            playerSprite->setPixmap(createPlayerSprite(m_state.animFrame, female));
        }
    } else if (m_state.animFrame != 0) {
        m_state.animFrame = 0;
        m_state.animTick  = 0;
        playerSprite->setPixmap(createPlayerSprite(0, female));
    }

    // ── Move enemies (polymorphic via Enemy::update()) ──────────────────────
    QPointF playerCenter = playerSprite->sceneBoundingRect().center();
    for (auto *e : m_enemies) {
        e->setPlayerPos(playerCenter);
        e->update();
    }

    // ── Update interactable animations (polymorphic via Interactable::update())
    for (auto *obj : m_interactables) {
        obj->setPlayerPos(playerCenter);
        obj->update();
    }

    // ── Invincibility flash ────────────────────────────
    if (m_state.invincible) {
        m_state.invFrames--;
        playerSprite->setOpacity((m_state.invFrames / 5) % 2 == 0 ? 1.0 : 0.25);
        if (m_state.invFrames <= 0) {
            m_state.invincible = false;
            playerSprite->setOpacity(1.0);
        }
    } else {
        // ── Enemy collision ─────────────────────────────
        QRectF pr = playerSprite->sceneBoundingRect();
        for (auto *e : m_enemies) {
            if (e->collidesWith(pr)) {
                onTrapHit();
                break;
            }
        }
    }

    // ── Room-specific mechanics ───────────────────────
    if (m_state.currentMechanic == RoomMechanic::SPIKE_TRAPS) {
        updateSpikeTraps();
    }
    if (m_state.currentMechanic == RoomMechanic::COLLAPSING_FLOOR) {
        updateCollapsingFloor();
    }
    if (m_state.currentMechanic == RoomMechanic::FAKE_DOORS) {
        checkFakeDoorCollision();
    }

    // ── Boss fight (room 5 only) ───────────────────────
    if (m_state.currentRoom == 5) {
        updateBoss();
        updateCrescents();
    }
}


// ═══════════════════════════════════════════════════════
// START GAME
// ═══════════════════════════════════════════════════════

void MainWindow::startGame()
{
    switch (m_player.difficulty) {
    case Difficulty::EASY: m_state.timeSeconds = 720; m_state.maxLives = 4; break;
    case Difficulty::HARD: m_state.timeSeconds = 300; m_state.maxLives = 2; break;
    default:               m_state.timeSeconds = 480; m_state.maxLives = 3; break;
    }

    m_state.lives           = m_state.maxLives;
    m_state.score           = 0;
    m_state.currentRoom     = 0;
    m_state.gameActive      = true;
    m_state.invincible      = false;
    m_state.invFrames       = 0;
    m_state.riddleOpen      = false;
    heldKeys.clear();

    m_state.bossState         = BossState::INACTIVE;
    m_state.bossHP            = 25;
    m_state.bossShootCount    = 0;
    m_state.bossShootTimer    = 0;
    m_state.bossTiredTimer    = 0;
    m_state.bossHitsThisPhase = 0;
    m_state.bossDyingTimer    = 0;

    m_state.cluesFound        = 0;
    m_state.requiredClues     = 3;
    m_state.relicsCollected   = 0;
    m_state.totalRelics       = 7;
    m_state.currentMechanic   = RoomMechanic::NONE;
    m_state.spikeTimer        = 0;

    m_mapLabels[5]->setText("Khonshu Boss");
    for (int i = 0; i < 6; i++) m_mapLabels[i]->setStyleSheet(
        "QLabel{background:rgba(15,10,5,180);color:#e2cfa0;"
        "border-left:3px solid #c8a03c;border-radius:6px;"
        "padding-left:8px;font-size:12px;font-family:\"Copperplate\";}");

    ui->inventorySlot1->setPixmap(QPixmap()); ui->inventorySlot1->setText("");
    ui->inventorySlot2->setPixmap(QPixmap()); ui->inventorySlot2->setText("");
    ui->inventorySlot3->setPixmap(QPixmap()); ui->inventorySlot3->setText("");
    ui->inventorySlot4->setPixmap(QPixmap()); ui->inventorySlot4->setText("");

    m_hud->refreshTimer(m_state.timeSeconds);
    m_hud->refreshLives(m_state.lives);
    m_hud->setScore(m_state.score);

    setupScene();
    loadRoom(0);

    countdownTimer->start();
    loopTimer->start();
    this->setFocus();
}


// ═══════════════════════════════════════════════════════
// RESET GAME
// ═══════════════════════════════════════════════════════

void MainWindow::resetGame()
{
    loopTimer->stop();
    countdownTimer->stop();
    m_state.gameActive = false;
    m_state.riddleOpen = false;
    heldKeys.clear();

    clearRoom();

    if (playerSprite && scene) {
        scene->removeItem(playerSprite);
        delete playerSprite;
        playerSprite = nullptr;
    }
}


// ═══════════════════════════════════════════════════════
// SCENE SETUP
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

    if (playerSprite) {
        scene->removeItem(playerSprite);
        delete playerSprite;
        playerSprite = nullptr;
    }

    const bool female = (m_player.character == "Zara" || m_player.character == "Nour");
    playerSprite = new QGraphicsPixmapItem(createPlayerSprite(0, female));
    playerSprite->setZValue(10);
    playerSprite->setPos(20, 173);
    scene->addItem(playerSprite);
}


// ═══════════════════════════════════════════════════════
// CLEAR ROOM
// ═══════════════════════════════════════════════════════

void MainWindow::clearRoom()
{
    // Boss crescents (not tracked in roomItems)
    for (auto &c : crescents) {
        if (scene) scene->removeItem(c.item);
        delete c.item;
    }
    crescents.clear();

    // Boss sprite
    if (bossSprite && scene) {
        scene->removeItem(bossSprite);
        delete bossSprite;
        bossSprite = nullptr;
    }
    m_state.bossState = BossState::INACTIVE;

    // Destructible covers
    for (auto &dc : destructibleCovers) {
        if (dc.item && scene) { scene->removeItem(dc.item); delete dc.item; }
    }
    destructibleCovers.clear();

    // Enemies — each destructor removes its items from the scene
    for (auto *e : m_enemies) delete e;
    m_enemies.clear();

    // Interactables — each destructor removes its items from the scene
    for (auto *obj : m_interactables) delete obj;
    m_interactables.clear();
    m_altar = nullptr;

    // Room-specific mechanics
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

    m_state.cluesFound = 0;

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
// LOAD ROOM
// ═══════════════════════════════════════════════════════

void MainWindow::loadRoom(int room)
{
    m_state.currentRoom = room;
    clearRoom();
    if (playerSprite) playerSprite->setPos(20, 173);

    showRoomIntro(room);

    static const QColor BG[7] = {
        {45,32,12}, {28,22,18}, {12,4,28},
        {4,12,32},  {38,24,4},  {5,3,22},
        {4,2,10}
    };
    scene->setBackgroundBrush(QBrush(BG[room]));

    addRoomRect(0, 360, 891, 61, QColor(0,0,0,80));

    m_state.requiredClues = (room == 5) ? 0 : 3;

    // ── ROOM 0: ENTRANCE CHAMBER ──────────────────────────────────────────────
    if (room == 0) {
        m_state.currentMechanic = RoomMechanic::TUTORIAL;

        addRoomRect(140, 40, 30, 150, QColor(110,85,45), QColor(150,120,70), 1.5, 1);
        addRoomRect(140, 220, 30, 130, QColor(110,85,45), QColor(150,120,70), 1.5, 1);
        addRoomRect(680, 40, 30, 130, QColor(110,85,45), QColor(150,120,70), 1.5, 1);
        addRoomRect(680, 200, 30, 150, QColor(110,85,45), QColor(150,120,70), 1.5, 1);
        addRoomRect(350, 100, 180, 180, QColor(60,45,25,100), QColor(100,80,50), 2, 1);
        addRoomText("ENTRANCE", 380, 15, QColor(180,150,80,160), 18, 1);

        addObstacle(250, 80, 40, 80, 0);
        addObstacle(550, 200, 50, 90, 0);
        addObstacle(300, 280, 45, 60, 0);

        addClueFragment(100, 60);
        addClueFragment(600, 300);
        addClueFragment(420, 140);
        addClueFragment(200, 250);

        addStatue(85, 270, "The statue depicts Ra, the Sun God. 'Seek wisdom in shadows.'");
        addStatue(750, 80, "An ancient pharaoh stands guard. 'Only the worthy may pass.'");

        addTablet(440, 280, "Tablet reads: 'Echoes speak without voice...'");

        addTorch(180, 150);

        addAltarToRoom(390, 180);

        ui->controlsLabel->setText("WASD Move | E Interact | Collect 3 clues to activate altar");
    }

    // ── ROOM 1: GUARDIAN HALL ────────────────────────────────────────────────
    else if (room == 1) {
        m_state.currentMechanic = RoomMechanic::STEALTH_GUARDIAN;

        addRoomRect(200, 20, 24, 340, QColor(70,60,50), QColor(100,90,80), 1, 1);
        addRoomRect(450, 20, 24, 340, QColor(70,60,50), QColor(100,90,80), 1, 1);
        addRoomRect(650, 20, 24, 340, QColor(70,60,50), QColor(100,90,80), 1, 1);
        addRoomText("GUARDIAN HALL", 290, 340, QColor(160,130,70,180), 16, 1);

        addObstacle(120, 60, 50, 70, 0);
        addObstacle(320, 180, 60, 50, 0);
        addObstacle(550, 80, 45, 80, 0);
        addObstacle(350, 290, 55, 50, 0);
        addObstacle(580, 260, 40, 70, 0);

        addEnemy(400, 140, 90, 120, true, 160, 680, 1.8f, 1.f, 0, 90.f, 2.5f);

        addClueFragment(90, 280);
        addClueFragment(520, 50);
        addClueFragment(720, 300);
        addClueFragment(280, 100);

        addStatue(140, 180, "A colossal guardian. 'Move when its gaze is elsewhere.'");

        addTablet(600, 180, "Carved warning: 'The guardian sees all who rush.'");
        addTablet(250, 300, "Hint: 'Footsteps leave traces behind...'");

        addTorch(85, 40);

        addRelic(750, 150, false);

        addAltarToRoom(380, 200);

        ui->controlsLabel->setText("WASD Move | E Interact | STEALTH: Stay behind cover!");
    }

    // ── ROOM 2: HALL OF ECHOES ───────────────────────────────────────────────
    else if (room == 2) {
        m_state.currentMechanic = RoomMechanic::FAKE_DOORS;

        for (int i = 0; i < 10; i++) {
            addRoomRect(80 + i * 85, 30 + (i % 4) * 75, 10, 10,
                        QColor(180, 100, 255, 80 + i * 15),
                        QColor(220, 160, 255, 150), 1, 1);
        }
        addRoomText("HALL OF ECHOES", 275, 340, QColor(180,100,255,180), 16, 1);

        addObstacle(150, 50, 30, 120, 0);
        addObstacle(280, 150, 35, 140, 0);
        addObstacle(450, 60, 30, 130, 0);
        addObstacle(580, 180, 35, 130, 0);
        addObstacle(350, 280, 100, 30, 0);

        addFakeDoor(200, 200);
        addFakeDoor(550, 80);

        addEnemy(320, 20, 55, 80, false, 20, 290, 2.2f, 1.f, 1, 0.f, 0.f);
        addEnemy(620, 120, 55, 80, false, 100, 320, 2.5f, -1.f, 1, 0.f, 0.f);

        addClueFragment(100, 200);
        addClueFragment(400, 100);
        addClueFragment(700, 280);
        addClueFragment(180, 320);

        addStatue(85, 60, "A ghostly figure. 'Not all paths lead forward.'");

        addTablet(680, 40, "Glyphs shimmer: 'The true path reveals itself to the patient.'");
        addTablet(130, 260, "Warning: 'Illusions guard this hall...'");

        addTorch(120, 320);

        addRelic(750, 320, false);

        addAltarToRoom(360, 150);

        ui->controlsLabel->setText("WASD Move | E Interact | BEWARE: Some doors are traps!");
    }

    // ── ROOM 3: TRIAL ROOM ───────────────────────────────────────────────────
    else if (room == 3) {
        m_state.currentMechanic = RoomMechanic::SPIKE_TRAPS;
        m_state.spikeTimer = 0;

        for (int i = 0; i < 6; i++) {
            addRoomRect(100 + i * 130, 345, 90, 12, QColor(60, 80, 180, 120),
                        QColor(100, 140, 255, 180), 1, 1);
        }
        addRoomText("TRIAL ROOM", 350, 15, QColor(100,140,255,180), 18, 1);

        addSpikeTrap(130, 90, 70, 40);
        addSpikeTrap(330, 90, 70, 40);
        addSpikeTrap(530, 90, 70, 40);
        addSpikeTrap(230, 190, 70, 40);
        addSpikeTrap(430, 190, 70, 40);
        addSpikeTrap(630, 190, 70, 40);
        addSpikeTrap(130, 290, 70, 40);
        addSpikeTrap(330, 290, 70, 40);
        addSpikeTrap(530, 290, 70, 40);

        addObstacle(180, 130, 50, 40, 0);
        addObstacle(380, 130, 50, 40, 0);
        addObstacle(580, 130, 50, 40, 0);
        addObstacle(280, 230, 50, 40, 0);
        addObstacle(480, 230, 50, 40, 0);

        addEnemy(200, 150, 70, 60, true, 100, 650, 2.0f, 1.f, 0, 0.f, 0.f);
        addEnemy(400, 260, 70, 60, true, 100, 650, 2.2f, -1.f, 0, 0.f, 0.f);

        addClueFragment(90, 170);
        addClueFragment(720, 100);
        addClueFragment(300, 320);
        addClueFragment(550, 300);

        addStatue(85, 40, "Ancient mechanism. 'Patience conquers the trial.'");

        addTablet(720, 260, "Instructions: 'Watch the rhythm of the stones.'");

        addTorch(680, 50);

        addRelic(720, 180, false);

        addAltarToRoom(350, 160);

        ui->controlsLabel->setText("WASD Move | E Interact | TIME the spikes carefully!");
    }

    // ── ROOM 4: ANKH SANCTUARY ───────────────────────────────────────────────
    else if (room == 4) {
        m_state.currentMechanic = RoomMechanic::HIDDEN_RELIC_PUZZLE;

        addRoomRect(720, 80, 100, 20, QColor(200, 160, 40, 180), QColor(255, 210, 80), 2, 1);
        addRoomRect(740, 40, 60, 45, QColor(220, 180, 60, 140), QColor(255, 220, 100), 2, 1);
        addRoomText("ANKH SANCTUARY", 285, 340, QColor(200,160,50,180), 16, 1);

        addObstacle(150, 40, 40, 150, 1);
        addObstacle(300, 120, 45, 180, 1);
        addObstacle(480, 40, 40, 140, 1);
        addObstacle(600, 150, 45, 160, 1);
        addObstacle(200, 280, 200, 35, 1);

        addEnemy(120, 280, 65, 65, true, 100, 320, 2.2f, 1.f, 2, 0.f, 0.f);
        addEnemy(550, 60, 65, 65, false, 40, 140, 2.5f, 1.f, 2, 0.f, 0.f);

        addClueFragment(100, 80);
        addClueFragment(550, 100);
        addClueFragment(250, 200);
        addClueFragment(700, 300);

        addStatue(85, 280, "The Ankh. 'Life eternal awaits the worthy.'");
        addStatue(650, 40, "Guardian of the sanctuary. 'Light reveals truth.'");

        addTablet(350, 50, "Inscribed: 'The sacred relic hides in darkness.'");

        addTorch(85, 60);

        addRelic(750, 250, false);

        addAltarToRoom(380, 180);

        ui->controlsLabel->setText("WASD Move | E Interact | Light the torch to reveal secrets!");
    }

    // ── ROOM 5: KHONSHU'S CHAMBER ────────────────────────────────────────────
    else if (room == 5) {
        m_state.currentMechanic = RoomMechanic::BOSS_FIGHT;

        for (int i = 0; i < 14; i++) {
            int sx = (i * 61 + 30) % 820;
            int sy = (i * 43 + 15) % 320;
            int sr = 2 + i % 4;
            addRoomRect(sx, sy, sr * 2, sr * 2,
                        QColor(200, 185, 255, 55 + i * 10));
        }

        addRoomText("KHONSHU'S CHAMBER", 240, 340,
                    QColor(175, 135, 255, 210), 16, 1);

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

        addRoomRect(326, 4, 206, 18, QColor(28, 10, 10, 210),
                    QColor(140, 50, 50), 2, 15);
        bossHPBar = addRoomRect(328, 6, 200, 14,
                                QColor(190, 42, 42, 230), Qt::transparent, 0, 16);
        addRoomText("KHONSHU HP", 232, 4, QColor(205, 155, 255, 230), 12, 15);

        bossSprite = new QGraphicsPixmapItem(createKhonshuSprite(120, 200));
        bossSprite->setPos(590, 90);
        bossSprite->setZValue(6);
        scene->addItem(bossSprite);

        m_state.bossState         = BossState::SHOOTING;
        m_state.bossHP            = 25;
        m_state.bossShootCount    = 0;
        m_state.bossShootTimer    = 0;
        m_state.bossTiredTimer    = 0;
        m_state.bossHitsThisPhase = 0;
        m_state.bossDyingTimer    = 0;

        ui->controlsLabel->setText(
            "WASD Move | R Strike Boss (when glowing) | ESC Pause");
    }

    // ── ROOM 6: ANUBIS GATE ──────────────────────────────────────────────────
    else if (room == 6) {
        m_state.currentMechanic = RoomMechanic::COLLAPSING_FLOOR;

        addRoomRect(760, 0, 131, 420, QColor(15, 8, 30, 200), QColor(80, 40, 120), 2, 1);
        addRoomRect(790, 20, 70, 140, QColor(30, 10, 60, 220), QColor(120, 60, 180), 2, 1);
        addRoomText("ANUBIS GATE", 330, 340, QColor(120,60,200,200), 18, 1);

        addCollapsingTile(120, 60, 55, 55);
        addCollapsingTile(280, 60, 55, 55);
        addCollapsingTile(440, 60, 55, 55);
        addCollapsingTile(200, 160, 55, 55);
        addCollapsingTile(360, 160, 55, 55);
        addCollapsingTile(520, 160, 55, 55);
        addCollapsingTile(120, 260, 55, 55);
        addCollapsingTile(280, 260, 55, 55);
        addCollapsingTile(440, 260, 55, 55);

        addObstacle(600, 80, 50, 60, 2);
        addObstacle(600, 200, 50, 60, 2);
        addObstacle(600, 300, 50, 60, 2);

        addEnemy(300, 120, 60, 60, true, 100, 550, 2.8f, 1.f, 3, 0.f, 0.f);
        addEnemy(400, 240, 60, 60, true, 100, 550, 3.0f, -1.f, 3, 0.f, 0.f);

        addClueFragment(90, 100);
        addClueFragment(550, 280);
        addClueFragment(350, 50);
        addClueFragment(180, 300);

        addStatue(700, 260, "Anubis awaits. 'Judge of the dead.'");

        addTablet(85, 200, "Final warning: 'The floor hungers for the unworthy.'");

        addTorch(85, 320);

        addRelic(500, 100, false);

        addAltarToRoom(650, 170);

        ui->controlsLabel->setText("WASD Move | E Interact | CAREFUL: The floor collapses!");
    }

    doorItem   = nullptr;
    pressEHint = nullptr;

    ui->roomNameLabel->setText(ROOMS[room].name);
    m_hud->setEventText(ROOMS[room].enterText);
    ui->levelLabel->setText(QString("%1 / 7").arg(room + 1));
    m_hud->refreshMap(m_state.currentRoom);
}


// ═══════════════════════════════════════════════════════
// TRY INTERACT  — E key; uses polymorphic Interactable::interact()
// ═══════════════════════════════════════════════════════

void MainWindow::tryInteract()
{
    if (!m_state.gameActive || m_state.riddleOpen || !playerSprite) return;

    QPointF pc = playerSprite->sceneBoundingRect().center();

    // Iterate in priority order: clues → statues → tablets → torches → relics → altar
    for (auto *obj : m_interactables) {
        if (!obj->isActive() || !obj->isNear(pc)) continue;

        InteractResult r = obj->interact();

        if (!r.eventText.isEmpty())
            m_hud->setEventText(r.eventText);

        if (r.clueCollected) {
            m_state.cluesFound++;
            m_hud->setEventText(QString("Clue fragment collected! (%1/%2)")
                .arg(m_state.cluesFound).arg(m_state.requiredClues));
            checkAltarActivation();
        }

        if (r.relicCollected) {
            m_state.relicsCollected++;
            m_state.score += r.scoreChange;
            m_hud->setScore(m_state.score);
            m_hud->setEventText(QString("Ancient relic found! (+50 points) [%1/%2 relics]")
                .arg(m_state.relicsCollected).arg(m_state.totalRelics));
        }

        if (r.torchActivated) {
            // Notify all interactables (RelicObj overrides reveal() to show itself)
            for (auto *o : m_interactables) o->reveal();
        }

        if (r.altarReady) {
            showRiddle();
        }

        return;
    }
}


// ═══════════════════════════════════════════════════════
// SHOW RIDDLE
// ═══════════════════════════════════════════════════════

void MainWindow::showRiddle()
{
    m_state.riddleOpen = true;
    loopTimer->stop();
    countdownTimer->stop();
    heldKeys.clear();

    RiddleDialog dlg(RIDDLES[m_state.currentRoom], m_player.trait, this);
    dlg.exec();

    if (!m_state.gameActive) return;

    countdownTimer->start();
    loopTimer->start();
    m_state.riddleOpen = false;

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
    const RoomInfo &r = ROOMS[m_state.currentRoom];
    int earned = r.baseScore;

    if      (m_player.trait == "Scholar")   earned += r.bonusScore;
    else if (m_player.trait == "Survivor")  earned += r.bonusScore - 10;
    else if (m_player.trait == "Speedster") {
        earned += r.bonusScore - 10;
        m_state.timeSeconds += 30;
        m_hud->refreshTimer(m_state.timeSeconds);
    }

    float mult = 1.f;
    if (m_player.difficulty == Difficulty::MEDIUM) mult = 1.5f;
    else if (m_player.difficulty == Difficulty::HARD) mult = 2.f;

    m_state.score += int(earned * mult);
    m_hud->setScore(m_state.score);

    if (r.survivorHeals && m_player.trait == "Survivor" && m_state.lives < m_state.maxLives) {
        m_state.lives++;
        m_hud->refreshLives(m_state.lives);
    }

    m_hud->setEventText(r.solvedText);

    if (m_state.currentRoom == 6) {
        triggerWin();
    } else {
        loadRoom(m_state.currentRoom + 1);
    }
}


// ═══════════════════════════════════════════════════════
// RIDDLE WRONG
// ═══════════════════════════════════════════════════════

void MainWindow::onRiddleWrong()
{
    m_state.lives--;
    m_hud->refreshLives(m_state.lives);

    if (m_state.lives <= 0) {
        triggerGameOver("You failed the ancient trial. The tomb claims you.");
        return;
    }

    if (playerSprite) playerSprite->setPos(20, 173);
    m_hud->setEventText("Wrong! The tomb punishes you.\nWalk back to the door and press E to try again.");
}


// ═══════════════════════════════════════════════════════
// TRAP HIT
// ═══════════════════════════════════════════════════════

void MainWindow::onTrapHit()
{
    if (m_state.invincible) return;

    if (m_player.trait == "Survivor" &&
        QRandomGenerator::global()->bounded(100) < 50) {
        m_state.invincible = true;
        m_state.invFrames  = 90;
        m_hud->setEventText("The trap strikes — your endurance holds! (Survivor)");
        return;
    }

    m_state.lives--;
    m_hud->refreshLives(m_state.lives);
    m_state.invincible = true;
    m_state.invFrames  = 120;

    if (m_state.lives <= 0)
        triggerGameOver("The tomb's traps have consumed you.");
    else
        m_hud->setEventText(QString("A trap strikes you! Lives remaining: %1").arg(m_state.lives));
}


// ═══════════════════════════════════════════════════════
// GAME OVER / WIN
// ═══════════════════════════════════════════════════════

void MainWindow::triggerGameOver(const QString &reason)
{
    loopTimer->stop();
    countdownTimer->stop();
    m_state.gameActive = false;

    ui->gameOverCauseLabel->setText(reason);
    ui->label_2->setText(QString::number(m_state.score));
    ui->stackedWidget->setCurrentWidget(ui->gameOverPage);
}

void MainWindow::triggerWin()
{
    loopTimer->stop();
    countdownTimer->stop();
    m_state.gameActive = false;

    m_state.score += m_state.timeSeconds;
    ui->winScoreLabel->setText(QString::number(m_state.score));
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

// addEnemy — replaces old addTrap(); dispatches to the correct Enemy subclass
void MainWindow::addEnemy(qreal x, qreal y, qreal w, qreal h,
                           bool horiz, float mn, float mx, float spd,
                           float startDir, int enemyType,
                           float detectRadius, float chaseSpd)
{
    Enemy *e = nullptr;
    switch (enemyType) {
    case 1:  e = new Specter  (scene, x, y, w, h, horiz, mn, mx, spd, startDir, detectRadius, chaseSpd); break;
    case 2:  e = new Scarab   (scene, x, y, w, h, horiz, mn, mx, spd, startDir, detectRadius, chaseSpd); break;
    case 3:  e = new DarkMummy(scene, x, y, w, h, horiz, mn, mx, spd, startDir, detectRadius, chaseSpd); break;
    default: e = new Mummy    (scene, x, y, w, h, horiz, mn, mx, spd, startDir, detectRadius, chaseSpd); break;
    }
    m_enemies.append(e);
}

void MainWindow::addObstacle(qreal x, qreal y, qreal w, qreal h, int style)
{
    QColor fill, border;
    switch (style) {
    case 1:
        fill   = QColor(138, 102, 34, 245);
        border = QColor(192, 152, 55);
        break;
    case 2:
        fill   = QColor(36, 26, 50, 248);
        border = QColor(72, 46, 108);
        break;
    default:
        fill   = QColor(90, 74, 50, 248);
        border = QColor(122, 100, 66);
        break;
    }

    auto *item = scene->addRect(x, y, w, h, QPen(border, 2), QBrush(fill));
    item->setZValue(3);
    roomItems.append(item);
    obstacles.append(item);

    if (w >= 22 && h >= 22) {
        int mx2 = int(x) + int(w) / 2 - 4, my2 = int(y) + int(h) / 2 - 4;
        auto *mark = scene->addRect(mx2, my2, 8, 8,
                                     QPen(border.darker(145), 1),
                                     QBrush(fill.darker(140)));
        mark->setZValue(4);
        roomItems.append(mark);
    }
}


// ═══════════════════════════════════════════════════════
// INTERACTABLE HELPERS  — wrap construction + registration
// ═══════════════════════════════════════════════════════

void MainWindow::addClueFragment(qreal x, qreal y)
{
    m_interactables.append(new ClueFragmentObj(x, y, scene));
}

void MainWindow::addStatue(qreal x, qreal y, const QString &lore)
{
    m_interactables.append(new StatueObj(x, y, lore, scene));
}

void MainWindow::addTablet(qreal x, qreal y, const QString &hint)
{
    m_interactables.append(new TabletObj(x, y, hint, scene));
}

void MainWindow::addTorch(qreal x, qreal y, QList<QGraphicsItem*> reveals)
{
    m_interactables.append(new TorchObj(x, y, scene, reveals));
}

void MainWindow::addRelic(qreal x, qreal y, bool startVisible)
{
    m_interactables.append(new RelicObj(x, y, startVisible, scene));
}

void MainWindow::addAltarToRoom(qreal x, qreal y)
{
    auto *altar = new AltarObj(x, y, scene);
    m_interactables.append(altar);
    m_altar = altar;   // convenience pointer for checkAltarActivation()
}

void MainWindow::checkAltarActivation()
{
    if (!m_altar || m_altar->isActive()) return;
    if (m_state.cluesFound >= m_state.requiredClues) {
        m_altar->activate(scene);
        m_hud->setEventText("The altar awakens! Approach and press E to face the riddle.");
    }
}


// ═══════════════════════════════════════════════════════
// BOSS FIGHT
// ═══════════════════════════════════════════════════════

void MainWindow::updateBoss()
{
    if (!bossSprite || m_state.bossState == BossState::INACTIVE) return;

    switch (m_state.bossState) {

    case BossState::SHOOTING:
        m_state.bossShootTimer++;
        if (m_state.bossShootTimer >= 45) {
            m_state.bossShootTimer = 0;
            fireCrescent();
            m_state.bossShootCount++;
            if (m_state.bossShootCount >= 8) {
                m_state.bossState         = BossState::TIRED;
                m_state.bossTiredTimer    = 600;
                m_state.bossHitsThisPhase = 0;
                m_state.bossShootCount    = 0;
                m_hud->setEventText("Khonshu is exhausted! Move close and press R to strike! (max 5 hits)");
            }
        }
        if (bossSprite)
            bossSprite->setOpacity(0.92 + 0.08 * qSin(m_state.bossShootTimer * 0.15));
        break;

    case BossState::TIRED:
    {
        m_state.bossTiredTimer--;
        double pulse = 0.45 + 0.40 * qSin(m_state.bossTiredTimer * 0.22);
        bossSprite->setOpacity(pulse);

        if (m_state.bossTiredTimer == 180)
            m_hud->setEventText("Khonshu is stirring — land your final hits now!");

        if (m_state.bossTiredTimer <= 0) {
            m_state.bossState = BossState::SHOOTING;
            m_state.bossShootTimer = 0;
            bossSprite->setOpacity(1.0);
            m_hud->setEventText("Khonshu rises again! Dodge the crescents!");
        }
        break;
    }

    case BossState::DYING:
        m_state.bossDyingTimer--;
        if (bossSprite)
            bossSprite->setOpacity(qMax(0.0, double(m_state.bossDyingTimer) / 90.0));
        if (m_state.bossDyingTimer <= 0) {
            m_state.bossState = BossState::INACTIVE;
            m_state.score += ROOMS[5].baseScore;
            m_hud->setScore(m_state.score);
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

        if (!sceneRect.intersects(c.item->sceneBoundingRect())) {
            scene->removeItem(c.item);
            delete c.item;
            crescents.remove(i);
            continue;
        }

        bool hitCover = false;
        for (int j = destructibleCovers.size() - 1; j >= 0; j--) {
            auto &dc = destructibleCovers[j];
            if (dc.item && c.item->sceneBoundingRect().intersects(dc.item->sceneBoundingRect())) {
                dc.hp--;
                if (dc.hp <= 0) {
                    scene->removeItem(dc.item);
                    delete dc.item;
                    destructibleCovers.remove(j);
                } else {
                    int alpha = 220 - (5 - dc.hp) * 35;
                    dc.item->setBrush(QBrush(QColor(80, 60, 140, alpha)));
                }
                scene->removeItem(c.item);
                delete c.item;
                crescents.remove(i);
                hitCover = true;
                break;
            }
        }
        if (hitCover) continue;

        if (!m_state.invincible && playerRect.intersects(c.item->sceneBoundingRect())) {
            scene->removeItem(c.item);
            delete c.item;
            crescents.remove(i);
            onTrapHit();
        }
    }
}

void MainWindow::tryBossHit()
{
    if (!m_state.gameActive || m_state.bossState != BossState::TIRED || !bossSprite || !playerSprite)
        return;

    if (m_state.bossHitsThisPhase >= 5) {
        m_hud->setEventText("Maximum hits reached for this phase — dodge until the next opening!");
        return;
    }

    QPointF pc = playerSprite->sceneBoundingRect().center();
    QPointF bc = bossSprite->sceneBoundingRect().center();
    float dist = qSqrt(qPow(pc.x() - bc.x(), 2) + qPow(pc.y() - bc.y(), 2));

    if (dist > 160.f) {
        m_hud->setEventText("Move closer to Khonshu and press R to strike!");
        return;
    }

    m_state.bossHitsThisPhase++;
    m_state.bossHP--;

    bossSprite->setOpacity(0.15);

    if (bossHPBar) {
        float ratio = float(m_state.bossHP) / 25.0f;
        bossHPBar->setRect(328, 6, qMax(0.f, 200.f * ratio), 14);
    }

    if (m_state.bossHP <= 0) {
        m_state.bossState      = BossState::DYING;
        m_state.bossDyingTimer = 90;
        for (auto &c : crescents) {
            scene->removeItem(c.item);
            delete c.item;
        }
        crescents.clear();
        m_hud->setEventText("KHONSHU FALLS! The Moon God crumbles into dust...");
    } else if (m_state.bossHitsThisPhase >= 5) {
        m_state.bossState = BossState::SHOOTING;
        m_state.bossShootTimer = 0;
        bossSprite->setOpacity(1.0);
        m_hud->setEventText("Khonshu recovers! Dodge the crescents!");
    } else {
        m_hud->setEventText(QString("Hit! Khonshu HP: %1 | Hits this phase: %2 / 5")
                     .arg(m_state.bossHP).arg(m_state.bossHitsThisPhase));
    }
}


// ═══════════════════════════════════════════════════════
// ROOM MECHANIC HELPERS
// ═══════════════════════════════════════════════════════

void MainWindow::addFakeDoor(qreal x, qreal y)
{
    auto *door = scene->addRect(x, y, 65, 100,
                                 QPen(QColor(255, 100, 50), 3),
                                 QBrush(QColor(140, 80, 20, 200)));
    door->setZValue(3);
    fakeDoors.append(door);

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
}

void MainWindow::addCollapsingTile(qreal x, qreal y, qreal w, qreal h)
{
    auto *tile = scene->addRect(x, y, w, h,
                                 QPen(QColor(100, 85, 55), 2),
                                 QBrush(QColor(85, 70, 45, 230)));
    tile->setZValue(2);
    collapsingTiles.append(tile);
    collapseTriggerTimer.append(-1);
    tileCollapsed.append(false);
}

void MainWindow::updateSpikeTraps()
{
    m_state.spikeTimer++;

    bool spikesUp = (m_state.spikeTimer % 120) < 60;

    for (int i = 0; i < spikeTraps.size(); i++) {
        bool thisUp = (i % 2 == 0) ? spikesUp : !spikesUp;
        spikeActive[i] = thisUp;

        QColor col = thisUp ? QColor(180, 50, 50, 220) : QColor(60, 45, 30, 180);
        spikeTraps[i]->setBrush(QBrush(col));

        if (thisUp && playerSprite && !m_state.invincible) {
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

        if (tileCollapsed[i]) {
            if (pr.intersects(tr) && !m_state.invincible) {
                onTrapHit();
                m_hud->setEventText("You fall through the broken floor!");
            }
            continue;
        }

        if (pr.intersects(tr)) {
            if (collapseTriggerTimer[i] < 0) {
                collapseTriggerTimer[i] = 90;
                collapsingTiles[i]->setBrush(QBrush(QColor(140, 80, 40, 230)));
            }
        }

        if (collapseTriggerTimer[i] > 0) {
            collapseTriggerTimer[i]--;

            int shake = (collapseTriggerTimer[i] % 6 < 3) ? 1 : -1;
            QRectF r = collapsingTiles[i]->rect();
            Q_UNUSED(r);
            collapsingTiles[i]->setPos(shake, 0);

            if (collapseTriggerTimer[i] <= 0) {
                tileCollapsed[i] = true;
                collapsingTiles[i]->setBrush(QBrush(QColor(20, 15, 10, 100)));
                collapsingTiles[i]->setPen(QPen(QColor(40, 30, 20, 100)));

                if (pr.intersects(tr) && !m_state.invincible) {
                    onTrapHit();
                    m_hud->setEventText("The floor gives way beneath you!");
                }
            }
        }
    }
}

void MainWindow::checkFakeDoorCollision()
{
    if (!playerSprite || m_state.invincible) return;

    QRectF pr = playerSprite->sceneBoundingRect();

    for (auto *fd : fakeDoors) {
        if (pr.intersects(fd->sceneBoundingRect())) {
            onTrapHit();
            m_hud->setEventText("A trap! That door was a deadly illusion!");
            return;
        }
    }
}


// ═══════════════════════════════════════════════════════
// ROOM INTRO
// ═══════════════════════════════════════════════════════

void MainWindow::showRoomIntro(int room)
{
    static const QString ROOM_STORIES[7] = {
        "ENTRANCE CHAMBER\n\n"
        "You descend into the ancient tomb, torchlight flickering against "
        "hieroglyph-covered walls. The air is thick with centuries of dust.\n\n"
        "Your mission: Find the scattered clue fragments hidden throughout "
        "each chamber. Once you collect enough, the central altar will "
        "awaken, presenting you with an ancient riddle.\n\n"
        "Explore carefully. The tomb holds many secrets.",

        "GUARDIAN HALL\n\n"
        "A massive stone guardian patrols these halls — the eternal protector "
        "of the pharaoh's secrets. Its glowing eyes scan for intruders.\n\n"
        "WARNING: The guardian can detect movement from great distances. "
        "Use the stone pillars for cover. Move only when its back is turned. "
        "If spotted, it will pursue relentlessly.\n\n"
        "Stealth is your ally here.",

        "HALL OF ECHOES\n\n"
        "Ghostly spectres drift through this ethereal chamber, remnants of "
        "souls who failed the tomb's trials. Strange illusions shimmer in "
        "the purple mist.\n\n"
        "BEWARE: Not all doors are what they seem. Some are deadly traps "
        "designed to punish the hasty. The spectres patrol their eternal "
        "routes but will not give chase.\n\n"
        "Trust your instincts. Find the true path.",

        "TRIAL ROOM\n\n"
        "Ancient mechanisms line the floor — pressure plates connected to "
        "deadly spike traps that rise and fall in an eternal rhythm.\n\n"
        "The spikes alternate in a pattern. Watch carefully, time your "
        "movements, and cross during the safe moments. The mummy guardians "
        "patrol but will not pursue you.\n\n"
        "Patience will see you through.",

        "ANKH SANCTUARY\n\n"
        "Golden light bathes this sacred chamber, home to the Ankh — "
        "symbol of eternal life. Scarab beetles patrol the sanctuary, "
        "sacred guardians of the ancient artifact.\n\n"
        "A hidden relic lies somewhere in the darkness. Light the torch "
        "to reveal what shadows conceal. The scarabs keep to their paths "
        "and will not give chase.\n\n"
        "The Ankh awaits the worthy.",

        "KHONSHU'S CHAMBER\n\n"
        "The Moon God Khonshu rises before you, his crescent staff "
        "glowing with divine power. This is no puzzle — this is battle.\n\n"
        "Khonshu hurls crescent projectiles at you. Dodge behind the "
        "stone pillars for cover. When he tires (he WILL glow and pulse), "
        "move close and press R to strike.\n\n"
        "You have 5 hits per opening. 25 hits total to defeat him.\n\n"
        "May the gods guide your hand.",

        "ANUBIS GATE\n\n"
        "The god of death waits at the final threshold. The very floor "
        "of this chamber crumbles beneath mortal feet.\n\n"
        "DANGER: Floor tiles collapse after you step on them. Move "
        "quickly — lingering means falling. Dark mummies guard the "
        "perimeter but will not pursue you.\n\n"
        "Find the final clues. Answer Anubis's riddle. Escape the tomb.\n\n"
        "This is your last trial."
    };

    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("Room " + QString::number(room + 1));
    dlg->setModal(true);
    dlg->setFixedSize(520, 360);
    dlg->setStyleSheet(
        "QDialog{background:#0d0a05;border:2px solid #c8a03c;}"
        "QLabel{color:#e2cfa0;font-family:'Copperplate';}"
        "QPushButton{background:#c8a03c;color:#0d0a05;border:none;"
        "padding:8px 24px;font-size:14px;font-family:'Copperplate';"
        "font-weight:bold;border-radius:4px;}"
        "QPushButton:hover{background:#e2c050;}");

    auto *layout = new QVBoxLayout(dlg);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto *lbl = new QLabel(ROOM_STORIES[room], dlg);
    lbl->setWordWrap(true);
    lbl->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    QFont f; f.setPixelSize(13);
    lbl->setFont(f);
    layout->addWidget(lbl, 1);

    auto *btn = new QPushButton("Enter the Tomb", dlg);
    connect(btn, &QPushButton::clicked, dlg, &QDialog::accept);
    layout->addWidget(btn, 0, Qt::AlignCenter);

    dlg->exec();
    delete dlg;
}
