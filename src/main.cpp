#include "GameMap.h"
#include "GameMapFactory.h"
#include "CreatureFactory.h"
#include "EventBus.h"
#include "EventProducer.h"
#include "FileExtractor.h"

#include <iostream>

int main(int ac, [[maybe_unused]] char **av)
{
    if (ac < 2)
    {
        std::cerr << "The first parameter should be a file path with game commands\n";
        std::exit(EXIT_FAILURE);
    }

    std::shared_ptr<GameMap> map;
    std::shared_ptr<EventBus> bus = std::make_shared<EventBus>();

    bus->Subscribe<MapCreationEvent>(
        [&map, &bus](auto&& event)
        {
            map = GameMapFactory::CreateMap(std::forward<decltype(event)>(event));
            bus->SetGameMap(map);
            map->SetEventBus(bus);
            bus->Subscribe<MarchEvent>(map);
            bus->Subscribe<FinishEvent>(map);
        });
    bus->Subscribe<SpawnCreatureEvent>(
        [](auto&& event)
        {
            CreatureFactory::CreateCreature(std::forward<decltype(event)>(event));
        });
    bus->SubscribeSelf<WaitEvent>();
    bus->SubscribeSelf<FinishEvent>();

    try
    {
        EventProducer producer(std::make_unique<FileExtractor>(av[1]));
        producer.Run(
            [bus](auto&& event)
            {
                bus->AddEvent(std::forward<decltype(event)>(event));
            });
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }

    bus->Join();
    map->Join();
}
