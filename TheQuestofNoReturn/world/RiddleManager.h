#ifndef RIDDLEMANAGER_H
#define RIDDLEMANAGER_H

#include <QObject>

class Player;
class Riddle;

class RiddleManager : public QObject {
    Q_OBJECT

public:
    explicit RiddleManager(QObject* parent = nullptr);

    void setPlayer(Player* player);

    void onRiddleSuccess(Riddle* riddle);
    void onRiddleFail();

private:
    Player* m_player = nullptr;
};

#endif
