#ifndef PAUSEMENUDIALOG_H
#define PAUSEMENUDIALOG_H

#include <QDialog>

namespace Ui {
class PauseMenuDialog;
}

class PauseMenuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PauseMenuDialog(QWidget *parent = nullptr);
    ~PauseMenuDialog();



signals:

    void resumeClicked();

    void restartClicked();

    void mainMenuClicked();

    void exitClicked();



private:

    Ui::PauseMenuDialog *ui;
};

#endif // PAUSEMENUDIALOG_H
