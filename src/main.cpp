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

    std::shared_ptr<GameMap> map;
    std::shared_ptr<EventBus> bus = std::make_shared<EventBus>(std::make_shared<StdOutPrinter>());

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
    bus->SubscribeSelf<FinishEvent>();

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

    // todo test producer
//    std::vector<std::shared_ptr<EventBase>> events = {
//        std::make_shared<MapCreationEvent>(MapPoint(40, 40)),
//        std::make_shared<SpawnCreatureEvent>(1, MapPoint(40, 40), 50),
//        std::make_shared<SpawnCreatureEvent>(2, MapPoint(30, 40), 100),
//        std::make_shared<SpawnCreatureEvent>(3, MapPoint(10, 20), 70),
//        std::make_shared<SpawnCreatureEvent>(4, MapPoint(40, 30), 90),
//        std::make_shared<MarchEvent>(1, MapPoint(40, 30)),
//        std::make_shared<MarchEvent>(2, MapPoint(20, 20)),
//        std::make_shared<WaitEvent>(1),
//        std::make_shared<WaitEvent>(1),
//        std::make_shared<WaitEvent>(1),
//        std::make_shared<WaitEvent>(1),
//        std::make_shared<WaitEvent>(1),
//        std::make_shared<WaitEvent>(1),
//        std::make_shared<WaitEvent>(1),
//        std::make_shared<WaitEvent>(1),
//        std::make_shared<WaitEvent>(1),
//        std::make_shared<WaitEvent>(1),
//        std::make_shared<MarchEvent>(4, MapPoint(20, 20)),
//        std::make_shared<FinishEvent>(),
//    };
//
//    for (auto& ev : events)
//    {
//        bus->AddEvent(std::move(ev));
//    }
}
