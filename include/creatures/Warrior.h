#pragma once

#include "base_interfaces/CreatureBase.h"

class Warrior : public BattleDispatcher<Warrior>
{
private:
    int m_Power;

public:
    explicit Warrior(int id, const MapPoint& coordinates, int power);

    BattleInfo BattleWith(const Warrior& enemy) const override;
};
