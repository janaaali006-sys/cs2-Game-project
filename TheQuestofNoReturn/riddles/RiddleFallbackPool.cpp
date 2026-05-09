#include "RiddleFallbackPool.h"
#include <QRandomGenerator>

RiddleFallbackPool::RiddleFallbackPool() {
    populate();
}

void RiddleFallbackPool::populate() {

    // ================= IDENTITY (6) =================
    m_pool[RiddleType::IDENTITY] = {
        new IdentityRiddle("I guard the horizon and rise every dawn. Who am I?", "Ra", "God of the sun", "Ra", Difficulty::EASY),
        new IdentityRiddle("I weigh the hearts of the dead.", "Anubis", "Jackal-headed god", "Anubis", Difficulty::MEDIUM),
        new IdentityRiddle("I rule the sky and storms.", "Horus", "Falcon god", "Horus", Difficulty::EASY),
        new IdentityRiddle("I am the keeper of knowledge and writing.", "Thoth", "Ibis-headed god", "Thoth", Difficulty::MEDIUM),
        new IdentityRiddle("I bring chaos and deserts.", "Set", "God of disorder", "Set", Difficulty::HARD),
        new IdentityRiddle("I protect the dead in the afterlife.", "Osiris", "Green-skinned god", "Osiris", Difficulty::MEDIUM)
    };

    // ================= LOGIC (6) =================
    m_pool[RiddleType::LOGIC] = {
        new LogicRiddle("If Ra rises in the east, where does he set?", "west", "Opposite direction", "Ra", Difficulty::EASY),
        new LogicRiddle("What comes after death in the judgment scale?", "heart", "Anubis weighs it", "Anubis", Difficulty::MEDIUM),
        new LogicRiddle("Three gods guard three doors. Only one leads to truth. Choose wisely.", "middle", "Balance matters", "Thoth", Difficulty::HARD),
        new LogicRiddle("If desert expands each year, what force rules it?", "set", "Chaos god", "Set", Difficulty::MEDIUM),
        new LogicRiddle("Light without sun exists where?", "underworld", "Duat realm", "Osiris", Difficulty::HARD),
        new LogicRiddle("What never dies in Egyptian myth?", "soul", "Ba or Ka", "Ra", Difficulty::EASY)
    };

    // ================= SEQUENCE (6) =================
    m_pool[RiddleType::SEQUENCE] = {
        new SequenceRiddle("Arrange: sunrise, noon, sunset", "sunrise-noon-sunset", "Time order of Ra", "Ra", Difficulty::EASY),
        new SequenceRiddle("Arrange: birth, judgment, afterlife", "birth-judgment-afterlife", "Soul journey", "Osiris", Difficulty::MEDIUM),
        new SequenceRiddle("Arrange symbols of Anubis ritual", "scale-heart-feather", "Judgment order", "Anubis", Difficulty::MEDIUM),
        new SequenceRiddle("Arrange Nile cycle events", "flood-plant-harvest", "Agriculture cycle", "Horus", Difficulty::EASY),
        new SequenceRiddle("Arrange god hierarchy", "ra-osiris-anubis", "Top to guide dead", "Thoth", Difficulty::HARD),
        new SequenceRiddle("Arrange temple ritual steps", "enter-pray-offer-exit", "Sacred flow", "Set", Difficulty::MEDIUM)
    };
}

Riddle* RiddleFallbackPool::getRandom(RiddleType type, QString /*godName*/) {
    auto &list = m_pool[type];
    if (list.isEmpty()) return nullptr;

    int index = QRandomGenerator::global()->bounded(list.size());
    return list[index];
}
