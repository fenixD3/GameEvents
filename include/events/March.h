#pragma once

#include "base_interfaces/EventBase.h"

class MarchEvent : public EventBase
{
private:
    int m_CreatureId;
    MapPoint m_Destination;

public:
    MarchEvent() = default;

    explicit MarchEvent(int creature_Id, const MapPoint& coordinates);

    std::string GetFiringMessage() const override;
    std::string GetFinishingMessage() const override;

    int GetCreatureId() const;
    MapPoint GetDestination() const;
};
