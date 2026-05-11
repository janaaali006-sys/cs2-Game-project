#include "artefactitem.h"
#include "Artefact.h"
#include "Room.h"
#include "player.h"
#include <QPixmap>
#include <QFileInfo>
#include <QPainter>

static QPixmap buildArtefactToken(ArtefactType type)
{
    QPixmap token(44, 44);
    token.fill(Qt::transparent);
    QPainter p(&token);
    p.setRenderHint(QPainter::Antialiasing, true);

    QColor core("#d6b15a");
    if (type == ArtefactType::HOURGLASS) core = QColor("#c58a48");
    if (type == ArtefactType::SCARAB_OF_RA) core = QColor("#d9c169");
    if (type == ArtefactType::SCROLL_OF_ANUBIS) core = QColor("#a67c52");
    if (type == ArtefactType::BLADE_OF_SEKHMET) core = QColor("#cf8a5b");

    p.setPen(QPen(QColor("#f5e0aa"), 2));
    p.setBrush(core);
    p.drawEllipse(4, 4, 36, 36);
    p.setPen(QPen(QColor("#4a2e12"), 2));
    p.drawLine(14, 22, 30, 22);
    p.drawLine(22, 14, 22, 30);
    p.end();
    return token;
}

ArtefactItem::ArtefactItem(Room* room, ArtefactType type, QGraphicsItem* parent)
    : GameObject(parent), m_room(room), m_type(type)
{
    QPixmap px = buildArtefactToken(m_type);
    if (px.isNull()) {
        px = QPixmap(28, 28);
        px.fill(QColor("#c8a03c"));
    }
    setPixmap(px.scaled(42, 42, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    setToolTip("Collect artefact: boosts score and grants a special effect.");
    setZValue(4);
}

void ArtefactItem::trigger(Player* player)
{
    if (m_triggered || !player)
        return;

    m_triggered = true;
    Artefact* artefact = createArtefact();
    if (!artefact)
        return;

    player->addArtefact(artefact);
    artefact->apply(player);
    player->addScore(50);
    hide();
}

QString ArtefactItem::objectName() const
{
    return "ArtefactItem";
}

Artefact* ArtefactItem::createArtefact() const
{
    switch (m_type) {
    case ArtefactType::EYE_OF_HORUS:     return new EyeOfHorus();
    case ArtefactType::HOURGLASS:        return new Hourglass();
    case ArtefactType::SCARAB_OF_RA:     return new ScarabOfRa();
    case ArtefactType::SCROLL_OF_ANUBIS: return new ScrollOfAnubis();
    case ArtefactType::BLADE_OF_SEKHMET: return new BladeOfSekhmet();
    default:                             return new EyeOfHorus();
    }
}

QString ArtefactItem::iconPath() const
{
    switch (m_type) {
    case ArtefactType::EYE_OF_HORUS:     return ":/images/scholarIcon.png";
    case ArtefactType::HOURGLASS:        return ":/images/speedsterIcon.png";
    case ArtefactType::SCARAB_OF_RA:     return ":/images/ankh.png";
    case ArtefactType::SCROLL_OF_ANUBIS: return ":/images/manuscript.png";
    case ArtefactType::BLADE_OF_SEKHMET: return ":/images/survivorIcon.png";
    default:                             return ":/images/ankh.png";
    }
}
