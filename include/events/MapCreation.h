#pragma once

#include "interfaces/IEvent.h"
#include "MapPoint.h"

class MapCreationEvent : public IEvent
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
