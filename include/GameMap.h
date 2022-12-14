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

namespace bmi = boost::multi_index;

class GameMap : public std::enable_shared_from_this<GameMap>
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
    class MarchDecorator;

private:
    MapPoint m_MapSize;
    std::atomic<double> m_GameTicks;

//    game_context_type m_GameContext;
//    std::unique_ptr<dummy_game_work_type> m_Work;
    std::thread m_GameThread;
    std::condition_variable m_MarchProcessing;
    bool m_Suspend;

    std::mutex m_CreaturesLocker;
    creature_container m_Creatures;

    std::mutex m_MarchLocker;
    std::priority_queue<MarchDecorator<MarchEvent>, std::vector<MarchDecorator<MarchEvent>>, std::greater<>> m_PendingMarches;

public:
    explicit GameMap(const MapPoint& map_size, const Key<GameMapFactory>&);
    ~GameMap();

    bool Include(const MapPoint& point) const;
    std::pair<bool, std::string> AddCreature(std::shared_ptr<CreatureBase>&& creature);
    void ProcessEvent(std::shared_ptr<MarchEvent> event);
    void ProcessEvent(std::shared_ptr<WaitEvent> event);
    void ProcessEvent(std::shared_ptr<FinishEvent> event);
    double GetGameTicks() const;

private:
    void ProcessBattleInfo(const BattleInfo& battle,
                           creature_container::index<IdTag>::type::iterator assaulter,
                           creature_container::index<PositionTag>::type::iterator defender);
    double AddTicks(double ticks);
};
