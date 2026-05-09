#ifndef PLAYER_H
#define PLAYER_H

#include <QString>
#include <QVector>
#include "SharedEnums.h"

class Artefact;

class Player {

public:
    Player();
    ~Player();

    // Lives
    int  getLives()      const { return m_lives; }
    void setLives(int v)       { m_lives = v; }
    void loseLife()            { if (m_lives > 0) m_lives--; }
    void gainLife()            { m_lives++; }

    // Score
    int  getScore()      const { return m_score; }
    void addScore(int v)       { m_score += v; }

    // Character info
    QString        getName()        const { return m_name; }
    void           setName(const QString& v) { m_name = v; }

    CharacterID    getCharacterID() const { return m_characterID; }
    void           setCharacterID(CharacterID v) { m_characterID = v; }

    CharacterTrait getTrait()       const { return m_trait; }
    void           setTrait(CharacterTrait v) { m_trait = v; }

    QString        getAvatarPath()  const { return m_avatarPath; }
    void           setAvatarPath(const QString& v) { m_avatarPath = v; }

    // Difficulty
    Difficulty getDifficulty() const { return m_difficulty; }
    void       setDifficulty(Difficulty v) { m_difficulty = v; }

    // Hint (Scholar trait gives 1 hint per level)
    bool hintAvailable()       const { return m_hintAvailable; }
    void setHintAvailable(bool v)    { m_hintAvailable = v; }

    // Scarab shield (blocks 1 life loss)
    bool hasScarabShield()     const { return m_scarabShield; }
    void setScarabShield(bool v)     { m_scarabShield = v; }

    // Inventory (max 4 artefacts)
    bool               addArtefact(Artefact* a); // replaces oldest if full
    bool               removeArtefact(ArtefactType type);
    bool               hasArtefact(ArtefactType type) const;
    Artefact*          getArtefact(ArtefactType type) const;
    QVector<Artefact*> getInventory() const { return m_inventory; }

    // Reset (called at game start)
    // Sets lives + hint based on difficulty and trait
    void reset();

    // Save / Load
    QString toJson()  const;
    void    fromJson(const QString& json);

private:
    int            m_lives         = 2;
    int            m_score         = 0;
    QString        m_name          = "Dr. Zara Khalil";
    CharacterID    m_characterID   = CharacterID::ZARA;
    CharacterTrait m_trait         = CharacterTrait::SCHOLAR;
    Difficulty     m_difficulty    = Difficulty::MEDIUM;
    QString        m_avatarPath    = "";
    bool           m_hintAvailable = false;
    bool           m_scarabShield  = false;
    QVector<Artefact*> m_inventory;
};

#endif // PLAYER_H
