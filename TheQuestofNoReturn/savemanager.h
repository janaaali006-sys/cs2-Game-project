#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <QString>
#include <QJsonObject>

class Player;

class SaveManager {
public:

    static bool saveGame(Player* player, int currentLevel);
    static bool loadGame(Player* player, int& currentLevel);
    static bool hasSaveFile();
    static void deleteSave();

private:
    static QString savePath();
};

#endif
