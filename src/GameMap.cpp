#include "GameMap.h"
#include "creatures/Warrior.h"

#include <boost/asio/post.hpp>

class GameMap::March
{
private:
    double m_MarchTicks;
    MapPoint m_Destination;

    size_t m_MarchCount;

public:
    March(double ticks, MapPoint dest, size_t count)
        : m_MarchTicks(ticks)
        , m_Destination(dest)
        , m_MarchCount(count)
    {}

    MapPoint GetDestination() const
    {
        return m_Destination;
    }

    friend bool operator>(const March& lhs, const March& rhs)
    {
        return std::isgreater(lhs.m_MarchTicks, rhs.m_MarchTicks);
    }
};

GameMap::GameMap(const MapPoint& map_size, const Key<GameMapFactory>&)
    : m_MapSize(map_size)
    , m_Work(std::make_unique<dummy_game_work_type>(m_GameContext.get_executor()))
{
    m_GameThread = std::thread([this] ()
    {
        m_GameContext.run();
    });
}

GameMap::~GameMap()
{
    m_Work.reset();
    m_GameThread.join();
}

bool GameMap::Include(const MapPoint& point) const
{
    return point.GetXCoord() > 0 && point.GetYCoord() > 0 && point.GetXCoord() <= m_MapSize.GetXCoord() && point.GetYCoord() <= m_MapSize.GetYCoord();
}

std::pair<bool, std::string> GameMap::AddCreature(std::shared_ptr<CreatureBase>&& creature)
{
    using namespace std::string_literals;

    if (m_Creatures.find(creature->GetId()) != m_Creatures.end())
    {
        return {false, "Creature with id "s + boost::lexical_cast<std::string>(creature->GetId()) + " already exists"};
    }
    if (m_Creatures.get<PositionTag>().find(creature->GetPosition()) != m_Creatures.get<PositionTag>().end())
    {
        return {false, "Creature on position "s + std::string(creature->GetPosition()) + " already exists"};
    }
    m_Creatures.insert(std::move(creature));
    return {true, ""};
}

void GameMap::ProcessEvent(std::shared_ptr<MarchEvent> event)
{
    using namespace std::string_literals;

    static size_t march_count = 0;

    if (!Include(event->GetMarchDestination()))
    {
        throw std::logic_error("March coordinates are out of map size");
    }

    auto creature = m_Creatures.find(event->GetCreatureId());
    if (creature == m_Creatures.end())
    {
        throw std::logic_error("Trying launch a march with non-existent creature with id "s + boost::lexical_cast<std::string>(event->GetCreatureId()));
    }

    m_PendingMarches.emplace((*creature)->GetPosition().LengthTo(event->GetMarchDestination()), event->GetMarchDestination(), march_count++);
    event->PrintMessage(event->GetFiringMessage());

    boost::asio::post(
        m_GameContext,
        [map = shared_from_this(), creature = *creature]()
        {
            auto active_march = map->m_PendingMarches.top();
            map->m_PendingMarches.pop();
            if (auto enemy = map->m_Creatures.get<PositionTag>().find(active_march.GetDestination()); enemy != map->m_Creatures.get<PositionTag>().end())
            {
                bool was_win = (*enemy)->Battle(creature);
            }
        });
}
