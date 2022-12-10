#pragma once

#include <memory>

class SpawnCreatureEvent;

class CreatureFactory
{
public:
    void CreateCreature(std::shared_ptr<SpawnCreatureEvent> event);
};