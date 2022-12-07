#include "events/SpawnCreature.h"

SpawnCreatureEvent::SpawnCreatureEvent(int creature_Id, MapPoint coordinates, int power)
    : m_CreatureId(creature_Id)
    , m_Coordinates(coordinates)
    , m_Power(power)
{}

std::string SpawnCreatureEvent::GetFiringMessage() const
{
    return IEvent::GetFiringMessage();
}

std::string SpawnCreatureEvent::GetFinishingMessage() const
{
    using namespace std::string_literals;
    return "WARRIOR SPAWNED "s + boost::lexical_cast<std::string>(m_CreatureId) + " ON " + boost::lexical_cast<std::string>(m_Coordinates);
}
