#ifndef HINTOUTCOME_H
#define HINTOUTCOME_H

#include "Outcome.h"

class HintOutcome : public Outcome {
public:
    HintOutcome(QString description);

    void apply(Player* player) override;
    QString getDescription() const override;
    bool isPositive() const override;

private:
    QString m_description;
};

#endif
