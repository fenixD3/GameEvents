#pragma once

#include "events/MapCreation.h"
#include "events/SpawnCreature.h"
#include "events/March.h"
#include "events/Wait.h"
#include "base_interfaces/CreatureBase.h"

#include <memory>
#include <thread>
#include <queue>

#include <iostream>

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

    class March;

private:
    game_context_type m_GameContext;
    std::unique_ptr<dummy_game_work_type> m_Work;
    std::thread m_GameThread;

    MapPoint m_MapSize;
    creature_container m_Creatures;

    std::priority_queue<March, std::vector<March>, std::greater<>> m_PendingMarches;

public:
    explicit GameMap(const MapPoint& map_size, const Key<GameMapFactory>&);
    ~GameMap();

    bool Include(const MapPoint& point) const;
    std::pair<bool, std::string> AddCreature(std::shared_ptr<CreatureBase>&& creature);
    void ProcessEvent(std::shared_ptr<MarchEvent> event);
    void ProcessEvent(std::shared_ptr<WaitEvent> event)
    {
        std::cout << "Wait\n";
    }
};
