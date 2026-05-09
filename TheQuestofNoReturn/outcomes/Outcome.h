#ifndef OUTCOME_H
#define OUTCOME_H

#include <QString>

class Player;

class Outcome {
public:
    virtual ~Outcome() {}

    virtual void apply(Player* player) = 0;
    virtual QString getDescription() const = 0;
    virtual bool isPositive() const = 0;
};

#endif
