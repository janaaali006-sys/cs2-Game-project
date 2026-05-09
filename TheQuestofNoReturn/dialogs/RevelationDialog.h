#ifndef REVELATIONDIALOG_H
#define REVELATIONDIALOG_H

#include <QDialog>

class Outcome;
class QLabel;
class QPushButton;

class RevelationDialog : public QDialog {
    Q_OBJECT

public:
    RevelationDialog(Outcome* outcome, QString artefactName = "", QWidget* parent = nullptr);

private:
    QLabel* m_banner;
    QLabel* m_description;
    QLabel* m_artefact;
    QPushButton* m_continueBtn;
};

#endif
