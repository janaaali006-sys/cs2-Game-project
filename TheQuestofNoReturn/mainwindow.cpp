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

static QPixmap createPlayerSprite(int frame = 0)
{
    QPixmap pix(55, 75);
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
    p.fillRect(14,  2, 27, 14, hat);
    // Hat brim (wide)
    p.fillRect( 2, 12, 51,  7, hat);
    // Hat band
    p.fillRect(14, 13, 27,  3, band);

    // Face
    p.fillRect(17, 20, 21, 17, skin);
    // Eyes
    p.fillRect(21, 25,  3,  3, eye);
    p.fillRect(31, 25,  3,  3, eye);
    // Mouth
    p.fillRect(23, 34,  9,  2, QColor(170, 70, 50));

    // Neck
    p.fillRect(23, 37,  9,  5, skin);

    // Torso
    p.fillRect(14, 42, 27, 16, shirt);
    // Left arm
    p.fillRect( 5, 42, 10, 14, shirt);
    // Right arm
    p.fillRect(40, 42, 10, 14, shirt);
    // Hands
    p.fillRect( 5, 55, 10,  5, skin);
    p.fillRect(40, 55, 10,  5, skin);

    // Belt
    p.fillRect(14, 58, 27,  3, QColor(50, 28, 6));
    // Buckle
    p.fillRect(24, 58,  7,  3, band);

    // Legs and boots: frame 0 = neutral, frame 1 = stride (legs spread)
    int lo = (frame == 0) ? 0 : 3;   // outward offset per leg
    p.fillRect(15 - lo, 61, 11, 12, pants);
    p.fillRect(29 + lo, 61, 11, 12, pants);
    p.fillRect(13 - lo, 70, 14,  5, boot);
    p.fillRect(28 + lo, 70, 14,  5, boot);

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
      "Ancient glyphs surround you. Reach the golden door and press E.",
      "The seal breaks — the passage is open!",
      false, 100, 60 },
    { "Guardian Hall",
      "A stone guardian patrols the hall. Dodge it and reach the door.",
      "The colossus crumbles — you may pass!",
      false, 120, 60 },
    { "Hall of Echoes",
      "Ethereal phantoms drift across the hall. Weave through and reach the door.",
      "The echoes answer your wisdom — the path clears!",
      false, 130, 60 },
    { "Trial Room",
      "Ancient mechanisms sweep the floor. Navigate them and answer the tablet.",
      "The stone tablet shatters — trial complete!",
      false, 150, 70 },
    { "Ankh Sanctuary",
      "Dark energy circles the altar. Avoid it and claim the Ankh.",
      "The Ankh glows — it has chosen you!",
      true, 140, 60 },
    { "Khonshu's Chamber",
      "The Moon God rises! Dodge the crescents — press R near Khonshu when he tires.",
      "Khonshu falls — the final gate lies ahead!",
      false, 300, 100 },
    { "Anubis Gate",
      "Anubis stands at the final gate. Prove your worth — reach him and answer.",
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
{
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

    // ── Move traps ─────────────────────────────────────
    for (auto &t : traps) {
        QPointF tp = t.item->pos();
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

    // ── Door proximity hint ────────────────────────────
    if (doorItem && pressEHint) {
        QPointF pc = playerSprite->sceneBoundingRect().center();
        QPointF dc = doorItem->sceneBoundingRect().center();
        float dist = qSqrt(qPow(pc.x()-dc.x(), 2) + qPow(pc.y()-dc.y(), 2));
        pressEHint->setVisible(dist < 130.f);
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

    for (auto &t : traps) {
        if (scene) scene->removeItem(t.item);
        delete t.item;
    }
    traps.clear();

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

    // Background colour per room (7 rooms: 0–5 normal + 5 boss + 6 Anubis)
    static const QColor BG[7] = {
        {45,32,12}, {28,22,18}, {12,4,28},
        {4,12,32},  {38,24,4},  {5,3,22},  // room 5: deep indigo moon-chamber
        {4,2,10}                             // room 6: Anubis Gate
    };
    scene->setBackgroundBrush(QBrush(BG[room]));

    // Floor strip
    addRoomRect(0, 360, 891, 61, QColor(0,0,0,80));

    // Room-specific atmosphere
    switch (room) {
    case 0:
        addRoomRect(140,40,30,310, QColor(110,85,45), QColor(150,120,70), 1.5, 1);
        addRoomRect(680,40,30,310, QColor(110,85,45), QColor(150,120,70), 1.5, 1);
        addRoomText("ENTRANCE", 330, 15, QColor(180,150,80,160), 18, 1);
        break;
    case 1:
        addRoomRect(200,20,20,340, QColor(70,60,50), QColor(100,90,80), 1, 1);
        addRoomRect(600,20,20,340, QColor(70,60,50), QColor(100,90,80), 1, 1);
        addRoomText("GUARDIAN HALL", 290, 340, QColor(160,130,70,180), 16, 1);
        break;
    case 2:
        for (int i = 0; i < 8; i++)
            addRoomRect(80+i*100, 30+(i%3)*80, 12, 12,
                        QColor(180,100,255, 100+i*18),
                        QColor(220,160,255,180), 1, 1);
        addRoomText("HALL OF ECHOES", 275, 340, QColor(180,100,255,180), 16, 1);
        break;
    case 3:
        for (int i = 0; i < 5; i++)
            addRoomRect(100+i*140,340,100,14, QColor(60,80,180,120),
                        QColor(100,140,255,180), 1, 1);
        addRoomText("TRIAL ROOM", 330, 15, QColor(100,140,255,180), 18, 1);
        break;
    case 4:
        addRoomRect(780,120,100,20, QColor(200,160,40,180), QColor(255,210,80), 2, 1);
        addRoomRect(800,80,60,42,   QColor(220,180,60,140), QColor(255,220,100), 2, 1);
        addRoomText("ANKH SANCTUARY", 265, 340, QColor(200,160,50,180), 16, 1);
        break;
    case 5: // Khonshu's Chamber — no regular enemies, no door; boss fight only
    {
        // Moonlit atmosphere: scattered star particles
        for (int i = 0; i < 14; i++) {
            int sx = (i * 61 + 30) % 820;
            int sy = (i * 43 + 15) % 320;
            int sr = 2 + i % 4;
            addRoomRect(sx, sy, sr * 2, sr * 2,
                        QColor(200, 185, 255, 55 + i * 10));
        }
        // Pillars the player can use for cover
        addObstacle(185, 20, 32, 320, 2);
        addObstacle(570, 20, 32, 320, 2);
        addRoomText("KHONSHU'S CHAMBER", 240, 340,
                    QColor(175, 135, 255, 210), 16, 1);

        // Boss HP bar (background + fill)
        addRoomRect(326, 4, 206, 18, QColor(28, 10, 10, 210),
                    QColor(140, 50, 50), 2, 15);
        bossHPBar = addRoomRect(328, 6, 200, 14,
                                QColor(190, 42, 42, 230), Qt::transparent, 0, 16);
        addRoomText("KHONSHU HP", 232, 4, QColor(205, 155, 255, 230), 12, 15);

        // Boss sprite (120×200 px, positioned center-right)
        bossSprite = new QGraphicsPixmapItem(createKhonshuSprite(120, 200));
        bossSprite->setPos(590, 90);
        bossSprite->setZValue(6);
        scene->addItem(bossSprite);

        // Initialise boss state machine
        bossState         = BossState::SHOOTING;
        bossHP            = 25;
        bossShootCount    = 0;
        bossShootTimer    = 0;
        bossTiredTimer    = 0;
        bossHitsThisPhase = 0;
        bossDyingTimer    = 0;

        ui->controlsLabel->setText(
            "WASD Move | R Strike Boss (when glowing) | ESC Pause");
        break;
    }

    case 6:
        addRoomRect(760,0,131,420, QColor(15,8,30,200), QColor(80,40,120), 2, 1);
        addRoomRect(790,20,70,140, QColor(30,10,60,220), QColor(120,60,180), 2, 1);
        addRoomText("ANUBIS GATE", 310, 340, QColor(120,60,200,200), 18, 1);
        ui->controlsLabel->setText("WASD Move | E Interact | ESC Pause");
        break;
    }

    // Golden door — only for non-boss rooms
    if (room != 5) {
        addRoomRect(793,152,79,116, QColor(255,200,50,40), QColor(255,200,50,100), 2, 2);
        doorItem = addRoomRect(800,160,65,100, QColor(180,130,20,200), QColor(255,210,60), 3, 3);
        addRoomText("ENTER", 808, 272, QColor(255,230,120), 13, 4);
    }

    // Enemies + Obstacles
    switch (room) {
    case 0: // Entrance — no enemies, no obstacles (tutorial)
        break;

    case 1: // Guardian Hall — mummy patrols full width
        addTrap(360,140,90,120, true, 160,680, 2.2f, 1.f, 0); // mummy
        addObstacle(415,  20, 38, 100, 0);   // fallen pillar top
        addObstacle(555, 250, 38,  95, 0);   // stone block bottom-right
        break;

    case 2: // Hall of Echoes — spectres drift vertically
        addTrap(280, 20,65, 90, false,  20,290, 2.5f,  1.f, 1); // specter
        addTrap(560,100,65, 90, false, 100,330, 3.0f, -1.f, 1); // specter
        addObstacle(320,  55, 28, 150, 0);   // sarcophagus left
        addObstacle(535, 165, 28, 145, 0);   // sarcophagus right
        break;

    case 3: // Trial Room — two mummies sweep horizontally
        addTrap(200,120,80, 70, true, 180,700, 2.8f,  1.f, 0); // mummy
        addTrap(200,250,80, 70, true, 180,700, 3.2f, -1.f, 0); // mummy
        addObstacle(375, 135, 48, 48, 0);   // rubble chunk A
        addObstacle(555, 225, 48, 48, 0);   // rubble chunk B
        break;

    case 4: // Ankh Sanctuary — two scarabs
        addTrap(160,180,75, 75, true,  140,620, 2.5f, 1.f, 2); // scarab
        addTrap(430, 40,75, 75, false,  40,300, 2.8f, 1.f, 2); // scarab
        addObstacle(455, 90, 34, 175, 1);   // golden altar pillar
        break;

    case 5: // Khonshu's Chamber — boss fight; no regular enemies
        break;

    case 6: // Anubis Gate — three dark mummies
        addTrap(160, 80,70, 70, true, 140,730, 3.0f,  1.f, 3); // dark mummy
        addTrap(160,190,70, 70, true, 140,730, 3.6f, -1.f, 3); // dark mummy
        addTrap(160,300,70, 70, true, 140,730, 2.8f,  1.f, 3); // dark mummy
        addObstacle(365,  40, 38,  92, 2);  // cursed dark stone A
        addObstacle(545, 250, 38,  88, 2);  // cursed dark stone B
        break;
    }

    // "Press E" hint above door — only for rooms with a door
    if (room != 5) {
        pressEHint = scene->addText("Press  E");
        pressEHint->setDefaultTextColor(QColor(255,255,120));
        QFont hf; hf.setPixelSize(14); hf.setBold(true);
        pressEHint->setFont(hf);
        pressEHint->setPos(794, 132);
        pressEHint->setZValue(20);
        pressEHint->setVisible(false);
        roomItems.append(pressEHint);
    }

    // Update HUD
    ui->roomNameLabel->setText(ROOMS[room].name);
    setEventText(ROOMS[room].enterText);
    ui->levelLabel->setText(QString("%1 / 7").arg(room + 1));
    refreshMap();
}


// ═══════════════════════════════════════════════════════
// TRY INTERACT  — E key handler
// ═══════════════════════════════════════════════════════

void MainWindow::tryInteract()
{
    if (!gameActive || riddleOpen || !playerSprite || !doorItem) return;

    QPointF pc = playerSprite->sceneBoundingRect().center();
    QPointF dc = doorItem->sceneBoundingRect().center();
    float dist = qSqrt(qPow(pc.x()-dc.x(),2) + qPow(pc.y()-dc.y(),2));

    if (dist < 130.f) showRiddle();
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

    if (currentRoom == 6)
        triggerWin();
    else
        loadRoom(currentRoom + 1);
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
                          float startDir, int enemyType)
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

    traps.append({item, sprite, horiz, mn, mx, spd, startDir, enemyType});
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
    } else {
        setEventText(QString("Hit! Khonshu HP: %1 | Hits this phase: %2 / 5")
                     .arg(bossHP).arg(bossHitsThisPhase));
    }
}
