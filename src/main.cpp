#include "GameMap.h"
#include "GameMapFactory.h"
#include "CreatureFactory.h"
#include "EventBus.h"
#include "StdOutPrinter.h"
#include "EventProducer.h"
#include "FileExtractor.h"

std::string GetFileName()
{
    static std::string name = "../logs/commented_logic";
    static int cnt = 0;

    while (std::filesystem::exists(name + boost::lexical_cast<std::string>(cnt) + ".log"))
    {
        ++cnt;
    }
    return name + boost::lexical_cast<std::string>(cnt) + ".log";
}

int main(int ac, [[maybe_unused]] char **av)
{
//    logging::configure({ {"type", "file"}, {"file_name", GetFileName()}, {"reopen_interval", "1"} });
    if (ac < 2)
    {
        std::cerr << "The first parameter should be a file path with game commands\n";
        std::exit(EXIT_FAILURE);
    }

    std::shared_ptr<GameMap> map;
    std::shared_ptr<EventBus> bus = std::make_shared<EventBus>();

    logging::INFO("Map and Bus were defined. Map is " + boost::lexical_cast<std::string>(map) + " doesn't initialize. Bus is " + boost::lexical_cast<std::string>(bus));

    bus->Subscribe<MapCreationEvent>(
        [&map, &bus](auto&& event)
        {
            logging::INFO("MapCreationEvent handler. ");
            map = GameMapFactory::CreateMap(std::forward<decltype(event)>(event));
            bus->SetGameMap(map);
            map->SetEventBus(bus);
            logging::INFO("Map created, map is " + boost::lexical_cast<std::string>(map));
            bus->Subscribe<MarchEvent>(map); // todo think about replace this to Game Map Factory
//            bus->Subscribe<WaitEvent>(map);
            bus->Subscribe<FinishEvent>(map);
        });
    bus->Subscribe<SpawnCreatureEvent>(
        [](auto&& event)
        {
            logging::INFO("SpawnCreatureEvent handler");
            CreatureFactory::CreateCreature(std::forward<decltype(event)>(event));
        });
    bus->SubscribeSelf<WaitEvent>();
    bus->SubscribeSelf<FinishEvent>();
//    bus->Subscribe<FinishEvent>(bus);

    // TODO core logic below!!!
/*    try
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
    }*/

    // todo test producer
    std::vector<std::shared_ptr<EventBase>> events = {
        std::make_shared<MapCreationEvent>(MapPoint(40, 40)),
        std::make_shared<SpawnCreatureEvent>(1, MapPoint(40, 40), 50),
        std::make_shared<SpawnCreatureEvent>(2, MapPoint(30, 40), 100),
        std::make_shared<SpawnCreatureEvent>(3, MapPoint(10, 20), 70),
        std::make_shared<SpawnCreatureEvent>(4, MapPoint(40, 30), 90),
        std::make_shared<MarchEvent>(2, MapPoint(20, 20)),
        std::make_shared<MarchEvent>(1, MapPoint(40, 30)),
        std::make_shared<WaitEvent>(10),
//        std::make_shared<WaitEvent>(1),
//        std::make_shared<WaitEvent>(1),
//        std::make_shared<WaitEvent>(1),
//        std::make_shared<WaitEvent>(1),
//        std::make_shared<WaitEvent>(1),
//        std::make_shared<WaitEvent>(1),
//        std::make_shared<WaitEvent>(1),
//        std::make_shared<WaitEvent>(1),
//        std::make_shared<WaitEvent>(1),
        std::make_shared<MarchEvent>(4, MapPoint(20, 20)),
        std::make_shared<FinishEvent>(),
    };

    for (auto& ev : events)
    {
        bus->AddEvent(std::move(ev));
    }

    logging::INFO("Map and Bus were created. Map is " + boost::lexical_cast<std::string>(map) + ". Bus is " + boost::lexical_cast<std::string>(bus));
}
