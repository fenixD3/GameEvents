#pragma once

#include "events/MapCreation.h"

#include <memory>

//template <typename TClass>
//class SingletonWrapper : public TClass
//{
//private:
//    static inline std::weak_ptr<TClass> m_Map;
//
//public:
//    static std::shared_ptr<TClass> Create()
//    {
//        auto instance = m_Map.lock();
//        if (!instance)
//        {
//            instance = std::make_shared<TClass>();
//            m_Map = instance;
//        }
//        return instance;
//    }
//};

class GameMap /*: public std::enable_shared_from_this<GameMap>*/
{
private:
    static inline std::weak_ptr<GameMap> Map;

    MapPoint m_MapSize;

public:
    static std::shared_ptr<GameMap> CreateMap(std::shared_ptr<MapCreationEvent> event);

private:
    GameMap(MapPoint map_size);
};
