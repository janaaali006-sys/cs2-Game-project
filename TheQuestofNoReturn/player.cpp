#include "player.h"
#include "Artefact.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// ============================================================
// Constructor / Destructor
// ============================================================

Player::Player(QObject* parent) : QObject(parent) {
    m_globalTimer = new QTimer(this);
    m_globalTimer->setInterval(1000);   // fires every 1 second
    connect(m_globalTimer, &QTimer::timeout, this, &Player::onTimerTick);
}

Player::~Player() {
    qDeleteAll(m_inventory);
}

// ============================================================
// Lives
// ============================================================

void Player::setLives(int v) {
    m_lives = v;
    emit livesChanged(m_lives);
}

void Player::loseLife() {
    // Scarab shield absorbs one hit, then breaks
    if (m_scarabShield) {
        m_scarabShield = false;
        return;
    }
    if (m_lives > 0) {
        m_lives--;
        emit livesChanged(m_lives);
    }
    if (m_lives == 0) {
        m_globalTimer->stop();
        emit gameOver();
    }
}

void Player::gainLife() {
    m_lives++;
    emit livesChanged(m_lives);
}

// ============================================================
// Score
// ============================================================

void Player::addScore(int v) {
    m_score += v;
    emit scoreChanged(m_score);
}

// ============================================================
// Timer
// ============================================================

void Player::startTimer() {
    m_globalTimer->start();
}

void Player::pauseTimer() {
    m_globalTimer->stop();
}

void Player::resumeTimer() {
    if (!m_globalTimer->isActive())
        m_globalTimer->start();
}

void Player::onTimerTick() {
    if (m_secondsRemaining > 0) {
        m_secondsRemaining--;
        emit timerTick(m_secondsRemaining);
    }
    if (m_secondsRemaining == 0) {
        m_globalTimer->stop();
        emit timerExpired();
        emit gameOver();
    }
}

// ============================================================
// Difficulty (applies defaults for lives + seconds + trait bonus)
// ============================================================

void Player::setDifficulty(Difficulty v) {
    m_difficulty = v;
    applyDifficultyDefaults();
}

void Player::applyDifficultyDefaults() {
    switch (m_difficulty) {
    case Difficulty::EASY:   m_lives = 3; m_secondsRemaining = 720; break;
    case Difficulty::MEDIUM: m_lives = 2; m_secondsRemaining = 480; break;
    case Difficulty::HARD:   m_lives = 1; m_secondsRemaining = 300; break;
    }
    // Trait bonuses on top
    if (m_trait == CharacterTrait::SURVIVOR)  m_lives++;
    if (m_trait == CharacterTrait::SPEEDSTER) m_secondsRemaining += 30;
}

// ============================================================
// Hint
// ============================================================

void Player::resetHintForLevel() {
    // Scholar gets one hint available at the start of each level
    m_hintAvailable = (m_trait == CharacterTrait::SCHOLAR);
}

// ============================================================
// Inventory
// ============================================================

bool Player::addArtefact(Artefact* a) {
    if (m_inventory.size() < 4) {
        m_inventory.append(a);
        return true;   // added without replacing
    }
    // Inventory full — replace the oldest (index 0)
    delete m_inventory.takeFirst();
    m_inventory.append(a);
    return false;      // false = replaced an old artefact
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

// ============================================================
// Reset (new game)
// ============================================================

void Player::reset() {
    m_score        = 0;
    m_scarabShield = false;
    qDeleteAll(m_inventory);
    m_inventory.clear();
    m_globalTimer->stop();

    applyDifficultyDefaults();  // sets lives + seconds + trait bonuses
    resetHintForLevel();
}

// ============================================================
// Save / Load
// ============================================================

QString Player::toJson() const {
    QJsonObject obj;
    obj["lives"]            = m_lives;
    obj["score"]            = m_score;
    obj["secondsRemaining"] = m_secondsRemaining;
    obj["name"]             = m_characterName;
    obj["avatarPath"]       = m_avatarPath;

    obj["difficulty"] = (m_difficulty == Difficulty::EASY)   ? "EASY" :
                        (m_difficulty == Difficulty::MEDIUM) ? "MEDIUM" : "HARD";

    obj["trait"]      = (m_trait == CharacterTrait::SCHOLAR)   ? "SCHOLAR" :
                        (m_trait == CharacterTrait::SURVIVOR)  ? "SURVIVOR" : "SPEEDSTER";

    obj["charID"]     = (m_characterID == CharacterID::ZARA)  ? "ZARA"  :
                        (m_characterID == CharacterID::KARIM) ? "KARIM" :
                        (m_characterID == CharacterID::NOUR)  ? "NOUR"  : "RAMI";

    // Inventory: save artefact type strings
    QJsonArray inv;
    for (auto* a : m_inventory) {
        switch (a->getType()) {
        case ArtefactType::EYE_OF_HORUS:      inv.append("EYE_OF_HORUS");      break;
        case ArtefactType::HOURGLASS:         inv.append("HOURGLASS");         break;
        case ArtefactType::SCARAB_OF_RA:      inv.append("SCARAB_OF_RA");      break;
        case ArtefactType::SCROLL_OF_ANUBIS:  inv.append("SCROLL_OF_ANUBIS");  break;
        case ArtefactType::BLADE_OF_SEKHMET:  inv.append("BLADE_OF_SEKHMET");  break;
        }
    }
    obj["inventory"] = inv;

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

void Player::fromJson(const QString& jsonStr) {
    QJsonObject obj = QJsonDocument::fromJson(jsonStr.toUtf8()).object();

    m_lives            = obj["lives"].toInt(2);
    m_score            = obj["score"].toInt(0);
    m_secondsRemaining = obj["secondsRemaining"].toInt(480);
    m_characterName    = obj["name"].toString();
    m_avatarPath       = obj["avatarPath"].toString();

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

    // Inventory is restored by the room system, not from JSON
    // (artefact objects live in the scene, not in the player JSON)
}
