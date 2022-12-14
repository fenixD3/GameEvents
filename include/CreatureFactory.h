#pragma once

#include <memory>

class SpawnCreatureEvent;

class CreatureFactory
{
public:
    static void CreateCreature(std::shared_ptr<SpawnCreatureEvent> event);
};
