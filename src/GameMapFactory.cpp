#include "GameMapFactory.h"
#include "GameMap.h"

std::shared_ptr<GameMap> GameMapFactory::CreateMap(std::shared_ptr<MapCreationEvent> event)
{
    if (auto instance = Map.lock(); !instance)
    {
        if (event->GetMapSize().GetXCoord() <= 0 || event->GetMapSize().GetYCoord() <= 0)
        {
            throw std::logic_error("Trying to create map with negative or zero size");
        }

        instance = std::make_shared<GameMap>(event->GetMapSize(), Key<GameMapFactory>{});
        Map = instance;
        event->PrintMessage(instance->GetGameTicks(), event->GetFinishingMessage());
        return instance;
    }
    throw std::logic_error("The second trying to construct game map");
}
