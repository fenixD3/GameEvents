#include "events/MapCreation.h"

MapCreationEvent::MapCreationEvent(const MapPoint& map_size)
    : m_MapSize(map_size)
{}

int MapCreationEvent::GetLength() const
{
    return m_MapSize.GetXCoord();
}

int MapCreationEvent::GetWidth() const
{
    return m_MapSize.GetYCoord();
}

MapPoint MapCreationEvent::GetMapSize() const
{
    return m_MapSize;
}

std::string MapCreationEvent::GetFiringMessage() const
{
    return EventBase::GetFiringMessage();
}

std::string MapCreationEvent::GetFinishingMessage() const
{
    using namespace std::string_literals;
    return "MAP CREATED "s + boost::lexical_cast<std::string>(GetLength()) + ' ' + boost::lexical_cast<std::string>(GetWidth());
}
