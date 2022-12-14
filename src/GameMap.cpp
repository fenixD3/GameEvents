#include "GameMap.h"
#include "creatures/Warrior.h"

#include <boost/asio/post.hpp>
#include <condition_variable>

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
    , m_Suspend(false)
//    , m_Work(std::make_unique<dummy_game_work_type>(m_GameContext.get_executor()))
{
    //logging::INFO("GameMap");
    m_GameThread = std::thread([this]()
    {
        auto extract_active_marches = [](auto& pending_marches, auto& inserter, double started_ticks)
        {
            while (!pending_marches.empty())
            {
                auto march = pending_marches.top();
                march.PrintMessage(started_ticks, march.GetFiringMessage());
                inserter.push_back(march);
                pending_marches.pop();
            }
        };

        //logging::INFO("Game Thread start");
//        m_GameContext.run();
        //logging::INFO("Game child Thread finish");
        while (true)
        {
            std::vector<MarchDecorator<MarchEvent>> active_marches;
            {
                std::unique_lock guard(m_MarchLocker);
                m_MarchProcessing.wait(guard, [this]{ return m_Suspend || !m_PendingMarches.empty(); });
                if (m_Suspend && m_PendingMarches.empty())
                {
                    //logging::INFO("Game child Thread finish");
                    break;
                }

                extract_active_marches(m_PendingMarches, active_marches, m_GameTicks);
            }

            double current_march_time = 0;
            for (auto& march : active_marches)
            {
                {
                    std::lock_guard guard(m_CreaturesLocker);
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
                }
                march.PrintMessage(AddTicks(march.GetMarchTime() - current_march_time), march.GetFinishingMessage());
                current_march_time += march.GetMarchTime();
            }
        }
    });
}

GameMap::~GameMap()
{
    //logging::INFO("~GameMap");
//    m_Work.reset(); // todo need transfer this to processing finish event?
    m_GameThread.join();
    //logging::INFO("~GameMap destroyed");
}

bool GameMap::Include(const MapPoint& point) const
{
    return point.GetXCoord() > 0 && point.GetYCoord() > 0 && point.GetXCoord() <= m_MapSize.GetXCoord() && point.GetYCoord() <= m_MapSize.GetYCoord();
}

std::pair<bool, std::string> GameMap::AddCreature(std::shared_ptr<CreatureBase>&& creature)
{
    using namespace std::string_literals;

    std::lock_guard guard(m_CreaturesLocker);
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
    //logging::INFO("MarchEvent handler");
    using namespace std::string_literals;

    static size_t march_count = 0;

    if (!Include(event->GetDestination()))
    {
        throw std::logic_error("March coordinates are out of map size");
    }

    decltype(m_Creatures.get<IdTag>().begin()) assaulter;
    {
        std::lock_guard guard(m_CreaturesLocker);
        assaulter = m_Creatures.find(event->GetCreatureId());
    }
    if (assaulter == m_Creatures.end())
    {
        throw std::logic_error("Trying launch a march with non-existent creature with id "s + boost::lexical_cast<std::string>(event->GetCreatureId()));
    }

    {
        std::lock_guard guard(m_MarchLocker);
        m_PendingMarches.emplace(march_count++, (*assaulter)->GetPosition(), *event);
    }

    //logging::INFO("Post March to game");
/*    boost::asio::post(
        m_GameContext,
        [this, assaulter]()
        {
            // todo need for emulating destroyed event bus before processing all events into game map worker
*//*            using namespace std::chrono_literals;
            std::this_thread::sleep_for(3s);*//*

            //logging::INFO("March processing. Game is " + boost::lexical_cast<std::string>(this));
            MarchDecorator<MarchEvent> active_march;
            {
                std::lock_guard guard(m_MarchLocker);
                active_march = m_PendingMarches.top();
                m_PendingMarches.pop();
            }

            {
                std::lock_guard guard(m_CreaturesLocker);
                if (auto defender = m_Creatures.get<PositionTag>().find(active_march.GetDestination()); defender != m_Creatures.get<PositionTag>().end())
                {
                    active_march.SetBattle((*defender)->Battle(*assaulter));
                    ProcessBattleInfo(*active_march.GetBattleInfo(), assaulter, defender);
                }
                else
                {
                    m_Creatures.get<PositionTag>().modify(
                        m_Creatures.project<PositionTag>(assaulter),
                        [&active_march](std::shared_ptr<CreatureBase>& modified) { modified->SetPosition(active_march.GetDestination()); });
                }
            }
            active_march.PrintMessage(m_GameTicks, active_march.GetFinishingMessage());
        });*/
}

void GameMap::ProcessEvent(std::shared_ptr<WaitEvent> event)
{
    //logging::INFO("WaitEvent handler");
    AddTicks(event->GetTicks());
    m_MarchProcessing.notify_one();
}

void GameMap::ProcessEvent([[maybe_unused]] std::shared_ptr<FinishEvent> event)
{
    //logging::INFO("FinishEvent handler");
    {
        std::lock_guard guard(m_MarchLocker);
        m_Suspend = true;
    }
    m_MarchProcessing.notify_one();
}

double GameMap::GetGameTicks() const
{
    return m_GameTicks;
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

double GameMap::AddTicks(double ticks)
{
    double current_tick = m_GameTicks;
    while (!m_GameTicks.compare_exchange_strong(current_tick, current_tick + ticks)) {}
    return current_tick + ticks;
}
