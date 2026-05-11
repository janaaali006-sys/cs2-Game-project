#ifndef ARTEFACTITEM_H
#define ARTEFACTITEM_H

#include "GameObject.h"
#include "SharedEnums.h"

class Room;
class Artefact;

class ArtefactItem : public GameObject
{
    Q_OBJECT
public:
    explicit ArtefactItem(Room* room, ArtefactType type, QGraphicsItem* parent = nullptr);
    void trigger(Player* player) override;
    QString objectName() const override;
    bool isPassable() const override { return true; }

private:
    Artefact* createArtefact() const;
    QString iconPath() const;

private:
    Room* m_room = nullptr;
    ArtefactType m_type = ArtefactType::EYE_OF_HORUS;
};

#endif // ARTEFACTITEM_H
