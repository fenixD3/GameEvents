#include "GameMap.h"
#include "GameMapFactory.h"
#include "CreatureFactory.h"
#include "EventBus.h"
#include "StdOutPrinter.h"
#include "EventProducer.h"
#include "FileExtractor.h"

#include "tlog.hpp"

int main(int ac, char **av)
{
    if (ac < 2)
    {
        std::cerr << "The first parameter should be a file path with game commands\n";
        std::exit(EXIT_FAILURE);
    }

    using namespace std::chrono_literals;
    std::shared_ptr<GameMap> map;
    std::shared_ptr<EventBus> bus = std::make_shared<EventBus>(std::make_unique<StdOutPrinter>());

    bus->Subscribe<MapCreationEvent>(
        [&map, &bus](auto&& event) // todo check about references in capture list
        {
            map = GameMapFactory{}.CreateMap(std::forward<decltype(event)>(event));
            bus->Subscribe<MarchEvent>(map); // todo think about replace this to Game Map Factory
        });
    bus->Subscribe<SpawnCreatureEvent>(
        [](auto&& event)
        {
            CreatureFactory{}.CreateCreature(std::forward<decltype(event)>(event));
        });
//    bus->Subscribe<WaitEvent>(bus); // todo it doesn't work too
//    bus->Subscribe<WaitEvent>( // todo why it doesn't work subscription to self slot??
//        [bus](auto&& event)
//        {
//            bus->ProcessEvent(std::forward<decltype(event)>(event));
//        });
    bus->SubscribeSelf<WaitEvent>(); // todo it works without shared_from_this!

    // TODO core logic below!!!
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
}
