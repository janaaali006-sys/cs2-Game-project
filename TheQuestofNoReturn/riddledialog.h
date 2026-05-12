#ifndef RIDDLEDIALOG_H
#define RIDDLEDIALOG_H

#include <QDialog>
#include <QString>
#include <QStringList>

class RiddleDialog : public QDialog
{
    Q_OBJECT
public:
    struct Riddle {
        QString     question;
        QStringList choices;       // exactly 4
        int         correctIndex;
        QString     scholarHint;   // shown only to Scholar trait
    };

    explicit RiddleDialog(const Riddle &riddle,
                          const QString &trait,
                          QWidget *parent = nullptr);

    bool wasCorrect() const { return m_correct; }

private:
    bool m_correct = false;
};

#endif // RIDDLEDIALOG_H
