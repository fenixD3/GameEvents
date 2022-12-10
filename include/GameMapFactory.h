#pragma once

#include <memory>

class GameMap;
class MapCreationEvent;

class GameMapFactory
{
    friend class CreatureFactory;
private:
    static inline std::weak_ptr<GameMap> Map;

public:
    std::shared_ptr<GameMap> CreateMap(std::shared_ptr<MapCreationEvent> event);
};
