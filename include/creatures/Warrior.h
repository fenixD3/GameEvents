#pragma once

#include "base_interfaces/CreatureBase.h"

class Warrior : public BattleDispatcher<Warrior>
{
private:
    int m_Power;

public:
    Warrior(int id, MapPoint coordinates, int power);

    bool BattleWith(const Warrior& enemy) const override;
};
