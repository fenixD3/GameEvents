#include "CreatureFactory.h"
#include "GameMapFactory.h"
#include "GameMap.h"
#include "creatures/Warrior.h"

void CreatureFactory::CreateCreature(std::shared_ptr<SpawnCreatureEvent> event)
{
    if (auto map = GameMapFactory::Map.lock(); !map)
    {
        throw std::logic_error("Trying to create creature before map");
    }
    else
    {
        //logging::INFO("Creates creature. Map is " + boost::lexical_cast<std::string>(map));
        if (!map->Include(event->GetCoordinates()))
        {
            throw std::logic_error("Creature coordinates are out of map size");
        }
        auto [was_added, error] = map->AddCreature(std::make_shared<Warrior>(event->GetCreatureId(), event->GetCoordinates(), event->GetPower()));
        if (!was_added)
        {
            throw std::logic_error(error);
        }
        event->PrintMessage(0, event->GetFinishingMessage());
    }
}
