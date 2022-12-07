#pragma once

#include "interfaces/IEvent.h"

class SpawnCreatureEvent : public IEvent
{
private:
    int m_CreatureId;
    MapPoint m_Coordinates;
    int m_Power;

public:
    SpawnCreatureEvent(int creature_Id, MapPoint coordinates, int power);

    std::string GetFiringMessage() const override;

    std::string GetFinishingMessage() const override;
};
