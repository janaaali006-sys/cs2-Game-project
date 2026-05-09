#include "savemanager.h"
#include "player.h"
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>


QString SaveManager::savePath() {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir + "/savegame.json";
}

bool SaveManager::hasSaveFile() {
    return QFile::exists(savePath());
}

void SaveManager::deleteSave() {
    QFile::remove(savePath());
}

bool SaveManager::saveGame(Player* player, int currentLevel) {
    if (!player) return false;

    // Get player's JSON representation
    QString playerJson = player->toJson();
    QJsonDocument playerDoc = QJsonDocument::fromJson(playerJson.toUtf8());
    QJsonObject saveObj = playerDoc.object();

    // Add level to the save object
    saveObj["currentLevel"] = currentLevel;

    QFile file(savePath());
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Could not open save file for writing:" << savePath();
        return false;
    }

    file.write(QJsonDocument(saveObj).toJson(QJsonDocument::Indented));
    file.close();
    qDebug() << "Game saved to:" << savePath();
    return true;
}

bool SaveManager::loadGame(Player* player, int& currentLevel) {
    if (!player || !hasSaveFile()) return false;

    QFile file(savePath());
    if (!file.open(QIODevice::ReadOnly)) return false;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qDebug() << "Save file is corrupt";
        return false;
    }

    QJsonObject obj = doc.object();
    currentLevel = obj["currentLevel"].toInt(1);

    // Remove currentLevel before passing to Player::fromJson
    obj.remove("currentLevel");
    player->fromJson(QJsonDocument(obj).toJson());

    qDebug() << "Game loaded. Level:" << currentLevel;
    return true;
}
