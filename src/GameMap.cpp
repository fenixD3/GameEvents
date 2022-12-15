#include "GameMap.h"
#include "EventBus.h"
#include "creatures/Warrior.h"

#include <boost/asio/post.hpp>
#include <condition_variable>
#include <utility>

template <typename TEvent>
class GameMap::MarchDecorator : public TEvent
{
private:
    double m_MarchTicks;
    std::optional<BattleInfo> m_Battle;

    size_t m_MarchCount;

public:
    MarchDecorator() = default;

    template <typename... TArgs>
    MarchDecorator(size_t count, const MapPoint& creature_position, TArgs&&... args)
        : TEvent(std::forward<TArgs>(args)...)
        , m_MarchCount(count)
    {
        m_MarchTicks = creature_position.LengthTo(GetDestination());
    }

    MapPoint GetDestination() const
    {
        return TEvent::GetDestination();
    }

    std::string GetFinishingMessage() const override
    {
        return TEvent::GetFinishingMessage() + ' ' + boost::lexical_cast<std::string>(m_Battle);
    }

    void SetBattle(const BattleInfo& battle)
    {
        m_Battle = battle;
    }

    const std::optional<BattleInfo>& GetBattleInfo() const
    {
        return m_Battle;
    }

    double GetMarchTime() const
    {
        return m_MarchTicks;
    }

    friend bool operator>(const MarchDecorator& lhs, const MarchDecorator& rhs)
    {
        return std::tie(lhs.m_MarchTicks, lhs.m_MarchCount) > std::tie(rhs.m_MarchTicks, rhs.m_MarchCount);
    }
};

GameMap::GameMap(const MapPoint& map_size, const Key<GameMapFactory>&)
    : m_MapSize(map_size)
    , m_GameTicks(0.0)
    , m_Work(std::make_unique<dummy_game_work_type>(m_GameContext.get_executor()))
{
    //logging::INFO("GameMap");
    m_GameThread = std::thread([this]()
    {
        //logging::INFO("Game Thread start");
        m_GameContext.run();
        //logging::INFO("Game child Thread finish");
    });
}

GameMap::~GameMap()
{
    //logging::INFO("~GameMap");
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
    //logging::INFO("MarchEvent handler GameMap");
    using namespace std::string_literals;

    static size_t march_count = 0;

    if (!Include(event->GetDestination()))
    {
        throw std::logic_error("March coordinates are out of map size");
    }

    auto assaulter = m_Creatures.find(event->GetCreatureId());
    if (assaulter == m_Creatures.end())
    {
        throw std::logic_error("Trying launch a march with non-existent creature with id "s + boost::lexical_cast<std::string>(event->GetCreatureId()));
    }

    m_PendingMarches.emplace(march_count++, (*assaulter)->GetPosition(), *event);
}

void GameMap::ProcessEvent(std::shared_ptr<WaitEvent> event)
{
    //logging::INFO("WaitEvent handler GameMap");
    double current_march_time = ProcessMarches();
    if (std::isgreater(event->GetTicks(), current_march_time))
    {
        m_GameTicks += event->GetTicks() - current_march_time;
    }

    //logging::INFO("Post WaitEvent finish to EventBus");
    if (auto bus = m_EventBus.lock(); bus)
    {
        boost::asio::post(
            bus->GetContext(),
            [bus, event = std::move(event)]()
            {
                bus->Continue();
            });
    }
    else
    {
        throw std::runtime_error("Can't notify EventBus about finishing WaitEvent");
    }
}

void GameMap::ProcessEvent([[maybe_unused]] std::shared_ptr<FinishEvent> event)
{
    //logging::INFO("Pot FinishEvent handler to GameMap");
    boost::asio::post(
        m_GameContext,
        [this, event]()
        {
            //logging::INFO("FinishEvent handler GameMap");
            ProcessMarches();
            event->PrintMessage(m_GameTicks, event->GetFinishingMessage());
            m_Work.reset();
        });
}

double GameMap::GetGameTicks() const
{
    return m_GameTicks;
}

void GameMap::SetEventBus(std::weak_ptr<EventBus> bus)
{
    m_EventBus = std::move(bus);
}

GameMap::game_context_type& GameMap::GetGameContext()
{
    return m_GameContext;
}

void GameMap::Join()
{
    //logging::INFO("Join GameMap");
    m_GameThread.join();
}

void GameMap::ProcessBattleInfo(const BattleInfo& battle,
                                creature_container::index<IdTag>::type::iterator assaulter,
                                creature_container::index<PositionTag>::type::iterator defender)
{
    if (!battle.GetWinner())
    {
        m_Creatures.erase(assaulter);
        m_Creatures.get<PositionTag>().erase(defender);
    }
    else if (*battle.GetWinner() == (*assaulter)->GetId())
    {
        m_Creatures.get<PositionTag>().erase(defender);
    }
    else
    {
        m_Creatures.erase(assaulter);
    }
}

double GameMap::ProcessMarches()
{
    std::vector<MarchDecorator<MarchEvent>> active_marches;
    //logging::INFO("March to process " + boost::lexical_cast<std::string>(m_PendingMarches.size()));
    while (!m_PendingMarches.empty())
    {
        auto march = m_PendingMarches.top();
        march.PrintMessage(m_GameTicks, march.GetFiringMessage());
        active_marches.push_back(march);
        m_PendingMarches.pop();
    }

    double current_march_time = 0;
    for (auto& march : active_marches)
    {
        auto assaulter = m_Creatures.find(march.GetCreatureId());
        if (auto defender = m_Creatures.get<PositionTag>().find(march.GetDestination()); defender != m_Creatures.get<PositionTag>().end())
        {
            march.SetBattle((*defender)->Battle(*assaulter));
            ProcessBattleInfo(*march.GetBattleInfo(), assaulter, defender);
        }
        else
        {
            m_Creatures.get<PositionTag>().modify(
                m_Creatures.project<PositionTag>(assaulter),
                [&march](std::shared_ptr<CreatureBase>& modified) { modified->SetPosition(march.GetDestination()); });
        }

        m_GameTicks += march.GetMarchTime() - current_march_time;
        march.PrintMessage(m_GameTicks, march.GetFinishingMessage());
        current_march_time += march.GetMarchTime() - current_march_time;
    }
    return current_march_time;
}
