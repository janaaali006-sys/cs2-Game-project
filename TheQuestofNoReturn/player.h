#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QTimer>
#include "SharedEnums.h"

class Artefact;

// ============================================================
// Player — owns game state, timer, and inventory.
// Inherits QObject so it can emit signals to HUD and Game.
// ============================================================
class Player : public QObject {
    Q_OBJECT

public:
    explicit Player(QObject* parent = nullptr);
    ~Player();

    // --- Lives ---
    int  getLives()  const { return m_lives; }
    void setLives(int v);
    void loseLife();          // emits livesChanged; emits gameOver if lives reach 0
    void gainLife();

    // --- Score ---
    int  getScore()      const { return m_score; }
    void addScore(int v);     // emits scoreChanged

    // --- Timer ---
    void startTimer();
    void pauseTimer();
    void resumeTimer();
    int  getSecondsRemaining() const { return m_secondsRemaining; }
    void addSeconds(int s)           { m_secondsRemaining += s; }  // used by Hourglass artefact

    // --- Character info ---
    QString        getCharacterName() const { return m_characterName; }
    void           setCharacterName(const QString& v) { m_characterName = v; }

    CharacterID    getCharacterID() const { return m_characterID; }
    void           setCharacterID(CharacterID v) { m_characterID = v; }

    CharacterTrait getTrait() const { return m_trait; }
    void           setTrait(CharacterTrait v) { m_trait = v; }

    QString        getAvatarPath() const { return m_avatarPath; }
    void           setAvatarPath(const QString& v) { m_avatarPath = v; }

    // --- Difficulty (also applies defaults) ---
    Difficulty getDifficulty() const { return m_difficulty; }
    void       setDifficulty(Difficulty v);   // calls applyDifficultyDefaults()

    // --- Hint (Scholar gets one per level) ---
    bool hintAvailable()         const { return m_hintAvailable; }
    void setHintAvailable(bool v)      { m_hintAvailable = v; }
    void resetHintForLevel();          // called by RoomManager at each new level

    // --- Scarab shield (blocks 1 life loss, set by ScarabOfRa artefact) ---
    bool hasScarabShield()  const { return m_scarabShield; }
    void setScarabShield(bool v)  { m_scarabShield = v; }

    // --- Inventory (max 4, replaces oldest when full) ---
    bool               addArtefact(Artefact* a);         // returns false if it replaced one
    bool               removeArtefact(ArtefactType type);
    bool               hasArtefact(ArtefactType type) const;
    Artefact*          getArtefact(ArtefactType type)  const;
    QVector<Artefact*> getInventory() const { return m_inventory; }

    // --- Full reset for new game ---
    void reset();

    // --- Save / Load ---
    QString toJson()  const;
    void    fromJson(const QString& json);

signals:
    void livesChanged(int newLives);
    void scoreChanged(int newScore);
    void timerTick(int secondsRemaining);
    void gameOver();
    void timerExpired();

private slots:
    void onTimerTick();

private:
    void applyDifficultyDefaults(); // sets lives + seconds from difficulty + trait

    int            m_lives            = 2;
    int            m_score            = 0;
    QString        m_characterName    = "Dr. Zara Khalil";
    CharacterID    m_characterID      = CharacterID::ZARA;
    CharacterTrait m_trait            = CharacterTrait::SCHOLAR;
    Difficulty     m_difficulty       = Difficulty::MEDIUM;
    QString        m_avatarPath       = "";
    bool           m_hintAvailable    = false;
    bool           m_scarabShield     = false;
    QVector<Artefact*> m_inventory;

    QTimer* m_globalTimer      = nullptr;
    int     m_secondsRemaining = 480;   // default = MEDIUM
};

#endif // PLAYER_H
