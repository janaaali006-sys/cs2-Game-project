#pragma once
#include <QObject>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
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
    Player* m_player;
    int m_speed = 5;
};
