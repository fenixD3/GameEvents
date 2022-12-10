#include "creatures/Warrior.h"

Warrior::Warrior(int id, const MapPoint& coordinates, int power)
    : BattleDispatcher(id, coordinates)
    , m_Power(power)
{}

BattleInfo Warrior::BattleWith(const Warrior& enemy) const
{
    BattleInfo info;
    info.SetAssaulter(GetId());
    info.SetDefender(enemy.GetId());
    if (m_Power > enemy.m_Power)
    {
        info.SetWinner(GetId());
    }
    else if (m_Power < enemy.m_Power)
    {
        info.SetWinner(enemy.GetId());
    }
    return info;
}
