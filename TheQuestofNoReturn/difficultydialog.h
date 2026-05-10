#ifndef DIFFICULTYDIALOG_H
#define DIFFICULTYDIALOG_H

#include <QDialog>
#include "SharedEnums.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class DifficultyDialog;
}
QT_END_NAMESPACE

class DifficultyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DifficultyDialog(QWidget *parent = nullptr);
    ~DifficultyDialog();

signals:
    void difficultySelected(Difficulty difficulty);

private:
    Ui::DifficultyDialog *ui;

    Difficulty selectedDifficulty;
};

#endif // DIFFICULTYDIALOG_H
