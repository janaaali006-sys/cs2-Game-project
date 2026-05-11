#ifndef AVATARGENERATOR_H
#define AVATARGENERATOR_H

#include <QNetworkAccessManager>
#include <QTimer>
#include "AIGenerator.h"

class AvatarGenerator : public AIGenerator {
    Q_OBJECT

public:
    explicit AvatarGenerator(QObject* parent = nullptr);

    void setApiKey(const QString& key) override  { m_apiKey    = key; }  // Gemini key
    void setOpenAiKey(const QString& key)         { m_openAiKey = key; }  // HuggingFace token

    void generateFromPhoto(const QString& photoPath);

signals:
    void avatarReady(QString path);
    void progressUpdate(QString message);
    // generationFailed() inherited from AIGenerator

private slots:
    void onTimeout();

private:
    void stepOneDescribe(const QString& base64Image, const QString& mimeType);
    void stepTwoRender(const QString& description);
    void stepThreeDownload(const QByteArray& imageData);  // HF returns bytes directly

    QString               m_openAiKey;   // holds HuggingFace token
    QString               m_photoPath;
    QNetworkAccessManager m_network;
    QTimer*               m_timeoutTimer = nullptr;
};

#endif // AVATARGENERATOR_H
