#ifndef LOSELIFEOUTCOME_H
#define LOSELIFEOUTCOME_H

#include "Outcome.h"

class LoseLifeOutcome : public Outcome {
public:
    LoseLifeOutcome(QString description);

    void apply(Player* player) override;
    QString getDescription() const override;
    bool isPositive() const override;

private:
    QString m_description;
};

#endif
