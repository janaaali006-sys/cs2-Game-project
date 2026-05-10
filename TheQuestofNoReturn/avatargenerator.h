#ifndef AVATARGENERATOR_H
#define AVATARGENERATOR_H

#include <QNetworkAccessManager>
#include <QTimer>
#include "AIGenerator.h"

// ============================================================
// AvatarGenerator — inherits AIGenerator.
//
// OOP role: Inheritance + Polymorphism
//   - Extends AIGenerator with avatar-specific generation.
//   - Overrides setApiKey() (pure virtual in AIGenerator).
//   - Adds a second key (OpenAI) for the DALL-E step.
//   - Multi-step async flow: Claude Vision → DALL-E 3 → download.
//
// Flow:
//   generateFromPhoto(path)
//     → stepOneDescribe()   : Claude Vision → appearance text
//     → stepTwoRender()     : DALL-E 3     → image URL
//     → stepThreeDownload() : download     → save PNG → emit avatarReady
// ============================================================
class AvatarGenerator : public AIGenerator {
    Q_OBJECT

public:
    explicit AvatarGenerator(QObject* parent = nullptr);

    // Implement pure virtual from AIGenerator (sets Claude key)
    void setApiKey(const QString& key) override { m_apiKey = key; }

    // Additional key needed for DALL-E 3
    void setOpenAiKey(const QString& key) { m_openAiKey = key; }

    // Main entry point — call with path to user's photo (JPEG or PNG)
    void generateFromPhoto(const QString& photoPath);

signals:
    void avatarReady(QString path);       // emitted with local PNG path on success
    // generationFailed() is inherited from AIGenerator
    void progressUpdate(QString message); // emitted at each step for UI feedback

private slots:
    void onTimeout();

private:
    void stepOneDescribe(const QString& base64Image, const QString& mimeType);
    void stepTwoRender(const QString& description);
    void stepThreeDownload(const QString& imageUrl);

    QString               m_openAiKey;
    QString               m_photoPath;
    QNetworkAccessManager m_network;
    QTimer*               m_timeoutTimer = nullptr;
};

#endif // AVATARGENERATOR_H
