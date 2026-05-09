#include "player.h"
#include "Artefact.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Player::Player() {}

Player::~Player() {
    qDeleteAll(m_inventory);
}

// Inventory

bool Player::addArtefact(Artefact* a) {
    if (m_inventory.size() < 4) {
        m_inventory.append(a);
        return true;
    }

    delete m_inventory.takeFirst();
    m_inventory.append(a);
    return false;
}

bool Player::removeArtefact(ArtefactType type) {
    for (int i = 0; i < m_inventory.size(); i++) {
        if (m_inventory[i]->getType() == type) {
            delete m_inventory.takeAt(i);
            return true;
        }
    }
    return false;
}

bool Player::hasArtefact(ArtefactType type) const {
    for (auto* a : m_inventory)
        if (a->getType() == type) return true;
    return false;
}

Artefact* Player::getArtefact(ArtefactType type) const {
    for (auto* a : m_inventory)
        if (a->getType() == type) return a;
    return nullptr;
}

// Reset

void Player::reset() {
    m_score         = 0;
    m_hintAvailable = false;
    m_scarabShield  = false;
    qDeleteAll(m_inventory);
    m_inventory.clear();

    switch (m_difficulty) {
    case Difficulty::EASY:   m_lives = 3; break;
    case Difficulty::MEDIUM: m_lives = 2; break;
    case Difficulty::HARD:   m_lives = 1; break;
    }

    if (m_trait == CharacterTrait::SURVIVOR) m_lives++;


    m_hintAvailable = (m_trait == CharacterTrait::SCHOLAR);
}

// Save / Load

QString Player::toJson() const {
    QJsonObject obj;
    obj["lives"] = m_lives;
    obj["score"] = m_score;
    obj["name"]  = m_name;
    obj["avatarPath"] = m_avatarPath;
    obj["difficulty"] = (m_difficulty == Difficulty::EASY)   ? "EASY" :
                            (m_difficulty == Difficulty::MEDIUM) ? "MEDIUM" : "HARD";
    obj["trait"]      = (m_trait == CharacterTrait::SCHOLAR)   ? "SCHOLAR" :
                       (m_trait == CharacterTrait::SURVIVOR)  ? "SURVIVOR" : "SPEEDSTER";
    obj["charID"]     = (m_characterID == CharacterID::ZARA)  ? "ZARA"  :
                        (m_characterID == CharacterID::KARIM) ? "KARIM" :
                        (m_characterID == CharacterID::NOUR)  ? "NOUR"  : "RAMI";
    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

void Player::fromJson(const QString& jsonStr) {
    QJsonObject obj = QJsonDocument::fromJson(jsonStr.toUtf8()).object();
    m_lives      = obj["lives"].toInt(2);
    m_score      = obj["score"].toInt(0);
    m_name       = obj["name"].toString();
    m_avatarPath = obj["avatarPath"].toString();

    QString d = obj["difficulty"].toString();
    m_difficulty = (d == "EASY") ? Difficulty::EASY :
                       (d == "HARD") ? Difficulty::HARD : Difficulty::MEDIUM;

    QString t = obj["trait"].toString();
    m_trait = (t == "SURVIVOR") ? CharacterTrait::SURVIVOR :
                  (t == "SPEEDSTER") ? CharacterTrait::SPEEDSTER : CharacterTrait::SCHOLAR;

    QString c = obj["charID"].toString();
    m_characterID = (c == "KARIM") ? CharacterID::KARIM :
                        (c == "NOUR")  ? CharacterID::NOUR  :
                        (c == "RAMI")  ? CharacterID::RAMI  : CharacterID::ZARA;
}
