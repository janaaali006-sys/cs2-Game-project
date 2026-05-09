#ifndef GOODOUTCOME_H
#define GOODOUTCOME_H

#include "Outcome.h"

class GoodOutcome : public Outcome {
public:
    GoodOutcome(QString description, int scoreBonus = 100);

    void apply(Player* player) override;
    QString getDescription() const override;
    bool isPositive() const override;

private:
    QString m_description;
    int m_scoreBonus;
};

#endif
