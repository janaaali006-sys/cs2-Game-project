#include "RiddleManager.h"

#include "../riddles/Riddle.h"
#include "../dialogs/DilemmaDialog.h"
#include "../dialogs/RevelationDialog.h"

#include "../outcomes/GoodOutcome.h"
#include "../outcomes/LoseLifeOutcome.h"
#include "../outcomes/HintOutcome.h"

RiddleManager::RiddleManager(QObject* parent)
    : QObject(parent)
{
}

void RiddleManager::setPlayer(Player* player) {
    m_player = player;
}

void RiddleManager::onRiddleSuccess(Riddle* /*riddle*/) {

    Outcome* a = new GoodOutcome("You solved the ancient trial.", 150);
    Outcome* b = new LoseLifeOutcome("The gods reject your hesitation.");
    Outcome* c = new HintOutcome("A hidden truth is revealed...");

    DilemmaDialog dilemma(
        "The temple reacts to your success. Choose your path.",
        a, b, c,
        m_player
    );

    dilemma.exec();

    RevelationDialog reveal(a, "", nullptr);
    reveal.exec();
}

void RiddleManager::onRiddleFail() {

    Outcome* fail = new LoseLifeOutcome("You failed the gods' challenge.");
    fail->apply(m_player);

    RevelationDialog reveal(fail, "", nullptr);
    reveal.exec();
}
