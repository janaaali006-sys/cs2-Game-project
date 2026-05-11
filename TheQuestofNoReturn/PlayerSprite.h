#pragma once
#include <QObject>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QTimer>
#include <QVector>
class Player;
class PlayerSprite : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit PlayerSprite(Player* player, QGraphicsItem* parent = nullptr);
    Player* playerData() const;
protected:
    void keyPressEvent(QKeyEvent* event) override;
    void advance(int phase) override;
private:
    QString spritePathFromCharacter() const;
    QVector<QPixmap> buildAnimationFrames(const QString& imagePath) const;
    void setFacingDirection(bool facingRight);
    void startWalkAnimationPulse();

private slots:
    void onAnimationTick();

private:
    Player* m_player;
    int m_speed = 8;
    QVector<QPixmap> m_frames;
    QTimer m_animTimer;
    int m_frameIndex = 0;
    int m_walkTicksRemaining = 0;
    bool m_facingRight = true;
};
