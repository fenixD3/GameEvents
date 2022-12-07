#pragma once

#include "interfaces/IEvent.h"

class MarchEvent : public IEvent
{
private:
    int m_CreatureId;
    MapPoint m_Coordinates;

public:
    MarchEvent(int creature_Id, MapPoint coordinates);

    std::string GetFiringMessage() const override;

    std::string GetFinishingMessage() const override;
};
