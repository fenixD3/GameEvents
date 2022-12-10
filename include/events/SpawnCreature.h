#pragma once

#include "base_interfaces/EventBase.h"

class SpawnCreatureEvent : public EventBase
{
private:
    int m_CreatureId;
    MapPoint m_Coordinates;
    int m_Power;

public:
    SpawnCreatureEvent(int creature_Id, MapPoint coordinates, int power);

    std::string GetFiringMessage() const override;
    std::string GetFinishingMessage() const override;

    int GetCreatureId() const;
    MapPoint GetCoordinates() const;
    int GetPower() const;
};
