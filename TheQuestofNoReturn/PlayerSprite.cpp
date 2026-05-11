#include "PlayerSprite.h"
#include "GameObject.h"
#include "player.h"
#include <QFileInfo>
#include <QGraphicsScene>
#include <QPainter>
#include <QPixmap>

QString PlayerSprite::spritePathFromCharacter() const
{
    if (!m_player)
        return ":/images/Zara.png";

    if (!m_player->getAvatarPath().isEmpty())
        return m_player->getAvatarPath();

    switch (m_player->getCharacterID()) {
    case CharacterID::KARIM: return ":/images/Karim.png";
    case CharacterID::NOUR:  return ":/images/NOOR.png";
    case CharacterID::RAMI:  return ":/images/RAMI.png";
    case CharacterID::ZARA:
    default:                 return ":/images/Zara.png";
    }
}

QVector<QPixmap> PlayerSprite::buildAnimationFrames(const QString& imagePath) const
{
    QPixmap source(imagePath);
    if (source.isNull() && imagePath.startsWith(":/images/")) {
        const QString baseName = imagePath.mid(QString(":/images/").size());
        const QString sourceDir = QFileInfo(QString::fromUtf8(__FILE__)).absolutePath();
        source.load(sourceDir + "/" + baseName);
    }
    if (source.isNull()) {
        source = QPixmap(44, 56);
        source.fill(QColor(70, 130, 200));
    }

    const int side = qMin(source.width(), source.height());
    const QRect squareRect((source.width() - side) / 2, (source.height() - side) / 2, side, side);
    const QPixmap head = source.copy(squareRect).scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QVector<QPixmap> frames;
    for (int i = 0; i < 2; ++i) {
        QPixmap canvas(46, 56);
        canvas.fill(Qt::transparent);

        QPainter painter(&canvas);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        // Build a simple avatar body so movement feels character-like, not a flat portrait.
        const int bob = (i == 0) ? 1 : -1;
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor("#d6b67a"));
        painter.drawRoundedRect(QRect(13, 24 + bob, 20, 17), 5, 5); // torso

        painter.setBrush(QColor("#8a5a2b"));
        painter.drawRect(QRect(14, 41 + bob, 6, 11));               // left leg
        painter.drawRect(QRect(26, 41 - bob, 6, 11));               // right leg

        painter.drawPixmap(QRect(11, 2 + bob, 24, 24), head);       // face/avatar head
        painter.end();

        frames.push_back(canvas);
    }

    return frames;
}

void PlayerSprite::setFacingDirection(bool facingRight)
{
    if (m_facingRight == facingRight)
        return;

    m_facingRight = facingRight;
    setTransform(QTransform::fromScale(m_facingRight ? 1.0 : -1.0, 1.0));
}

void PlayerSprite::startWalkAnimationPulse()
{
    m_walkTicksRemaining = 6;
    if (!m_animTimer.isActive())
        m_animTimer.start();
}

PlayerSprite::PlayerSprite(Player* player, QGraphicsItem* parent)
    : QObject(), QGraphicsPixmapItem(parent), m_player(player)
{
    m_frames = buildAnimationFrames(spritePathFromCharacter());
    setPixmap(m_frames.isEmpty() ? QPixmap() : m_frames.first());

    m_animTimer.setInterval(130);
    connect(&m_animTimer, &QTimer::timeout, this, &PlayerSprite::onAnimationTick);
    m_animTimer.start();

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}
Player* PlayerSprite::playerData() const
{
    return m_player;
}
void PlayerSprite::keyPressEvent(QKeyEvent* event)
{
    if (!scene())
        return;

    QPointF oldPos = pos();
    bool moved = false;

    if (event->key() == Qt::Key_Left) {
        setX(x() - m_speed);
        setFacingDirection(false);
        moved = true;
    } else if (event->key() == Qt::Key_Right) {
        setX(x() + m_speed);
        setFacingDirection(true);
        moved = true;
    } else if (event->key() == Qt::Key_Up) {
        setY(y() - m_speed);
        moved = true;
    } else if (event->key() == Qt::Key_Down) {
        setY(y() + m_speed);
        moved = true;
    }

    QRectF bounds = scene()->sceneRect();
    if (!bounds.contains(sceneBoundingRect())) {
        setPos(oldPos);
        moved = false;
    }

    if (moved)
        startWalkAnimationPulse();
}

void PlayerSprite::advance(int phase)
{
    if (phase == 0 || !scene() || !m_player)
        return;
    QList<QGraphicsItem*> collisions = scene()->collidingItems(this);
    for (QGraphicsItem* item : collisions) {
        GameObject* object = dynamic_cast<GameObject*>(item);
        if (object && !object->isTriggered()) {
            object->trigger(m_player);
        }
    }
}

void PlayerSprite::onAnimationTick()
{
    if (m_frames.isEmpty())
        return;

    if (m_walkTicksRemaining > 0) {
        m_walkTicksRemaining--;
        m_frameIndex = (m_frameIndex + 1) % m_frames.size();
    } else {
        m_frameIndex = 0;
    }

    setPixmap(m_frames[m_frameIndex]);
}
