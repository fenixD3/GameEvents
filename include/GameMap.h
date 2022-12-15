#pragma once

#include "events/MapCreation.h"
#include "events/SpawnCreature.h"
#include "events/March.h"
#include "events/Wait.h"
#include "events/Finish.h"
#include "base_interfaces/CreatureBase.h"

#include <memory>
#include <thread>
#include <queue>
#include <mutex>

#include <boost/asio/io_context.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/key.hpp>

template <typename T>
class Key
{
private:
    friend T;
    Key() {}
    Key(const Key&) = delete;
};

class GameMapFactory;
class EventBus;

namespace bmi = boost::multi_index;

class GameMap
{
private:
    using game_context_type = boost::asio::io_context;
    using dummy_game_work_type = boost::asio::executor_work_guard<decltype(std::declval<game_context_type>().get_executor())>;

    struct IdTag {};
    struct PositionTag {};
    using creature_container = boost::multi_index_container<
        std::shared_ptr<CreatureBase>,
        bmi::indexed_by<
            bmi::hashed_unique<bmi::tag<IdTag>, bmi::key<&CreatureBase::GetId>>,
            bmi::hashed_unique<bmi::tag<PositionTag>, bmi::key<&CreatureBase::GetPosition>, MapPointHasher>
        >
    >;

    template <typename TEvent>
    class MarchDecorator : public TEvent
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

private:
    MapPoint m_MapSize;
    double m_GameTicks;

    game_context_type m_GameContext;
    std::unique_ptr<dummy_game_work_type> m_Work;
    std::thread m_GameThread;

    creature_container m_Creatures;

    std::priority_queue<MarchDecorator<MarchEvent>, std::vector<MarchDecorator<MarchEvent>>, std::greater<>> m_PendingMarches;

    std::weak_ptr<EventBus> m_EventBus;

public:
    explicit GameMap(const MapPoint& map_size, const Key<GameMapFactory>&);

    bool Include(const MapPoint& point) const;
    std::pair<bool, std::string> AddCreature(std::shared_ptr<CreatureBase>&& creature);
    void ProcessEvent(std::shared_ptr<MarchEvent> event);
    void ProcessEvent(std::shared_ptr<WaitEvent> event);
    void ProcessEvent(std::shared_ptr<FinishEvent> event);
    double GetGameTicks() const;

    void SetEventBus(std::weak_ptr<EventBus> bus);
    game_context_type& GetGameContext();
    void Join();

private:
    void ProcessBattleInfo(const BattleInfo& battle,
                           creature_container::index<IdTag>::type::iterator assaulter,
                           creature_container::index<PositionTag>::type::iterator defender);
    double ProcessMarches();
};
