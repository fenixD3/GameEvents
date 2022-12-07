#include "GameMap.h"

#include <iostream>

std::shared_ptr<GameMap> GameMap::CreateMap(std::shared_ptr<MapCreationEvent> event)
{
//    struct GameMapHelper : public GameMap { using GameMap::GameMap; };

    auto instance = Map.lock();
    if (!instance)
    {
//        instance = std::make_shared<GameMapHelper>(event->GetMapSize());
        instance = std::shared_ptr<GameMap>(new GameMap(event->GetMapSize()));
        Map = instance;
        return instance;
    }
    throw std::logic_error("The second trying to construct game map");
}

GameMap::GameMap(MapPoint map_size)
    : m_MapSize(map_size)
{
    std::cout << "Map creation\n";
    // TODO need start new thread and boost.asio
}
