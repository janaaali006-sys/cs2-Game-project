#ifndef AIGENERATOR_H
#define AIGENERATOR_H

#include <QObject>
#include <QString>

// ============================================================
// AIGenerator — abstract base class for AI-powered generators.
//
// OOP role: Inheritance + Abstraction + Polymorphism (Person 5)
//   - Both RiddleGenerator and AvatarGenerator follow the same
//     pattern: take input → call Claude API → emit result.
//   - This base class defines that shared interface.
//   - Subclasses override generate() with their specific logic.
//   - Demonstrates that inheritance isn't only for game objects —
//     it applies to service/utility classes too.
//
// Hierarchy:
//   AIGenerator (abstract)
//   ├── RiddleGenerator   → generates Riddle* via Claude API
//   └── AvatarGenerator   → generates avatar PNG via Claude + DALL-E
// ============================================================
class AIGenerator : public QObject {
    Q_OBJECT

public:
    explicit AIGenerator(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~AIGenerator() {}

    // Set the Claude API key (both subclasses need this)
    virtual void setApiKey(const QString& key) = 0;

    // Whether to actually call the AI or use fallback/offline mode
    void setUseAI(bool val) { m_useAI = val; }
    bool getUseAI() const   { return m_useAI; }

signals:
    // Emitted by subclasses when generation fails for any reason
    void generationFailed();

protected:
    bool    m_useAI  = true;
    QString m_apiKey;
};

#endif // AIGENERATOR_H
