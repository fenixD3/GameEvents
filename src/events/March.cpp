#include "events/March.h"

#include <boost/lexical_cast.hpp>

MarchEvent::MarchEvent(int creature_Id, const MapPoint& coordinates)
    : m_CreatureId(creature_Id)
    , m_Destination(coordinates)
{}

std::string MarchEvent::GetFiringMessage() const
{
    using namespace std::string_literals;
    return "MARCH STARTED "s + boost::lexical_cast<std::string>(m_CreatureId) + " TO " + boost::lexical_cast<std::string>(m_Destination);
}

std::string MarchEvent::GetFinishingMessage() const
{
    using namespace std::string_literals;
    return "MARCH "s + boost::lexical_cast<std::string>(m_CreatureId) + " FINISHED " + boost::lexical_cast<std::string>(m_Destination);
}

int MarchEvent::GetCreatureId() const
{
    return m_CreatureId;
}

MapPoint MarchEvent::GetDestination() const
{
    return m_Destination;
}
