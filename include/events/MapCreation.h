#pragma once

#include "base_interfaces/EventBase.h"
#include "MapPoint.h"

class MapCreationEvent : public EventBase
{
private:
    MapPoint m_MapSize;

public:
    explicit MapCreationEvent(MapPoint map_size);

    std::string GetFiringMessage() const override;

    std::string GetFinishingMessage() const override;

    int GetLength() const;
    int GetWidth() const;
    MapPoint GetMapSize() const;
};
