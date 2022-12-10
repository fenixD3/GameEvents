#include "creatures/Warrior.h"

Warrior::Warrior(int id, MapPoint coordinates, int power)
    : BattleDispatcher(id, coordinates)
    , m_Power(power)
{}

bool Warrior::BattleWith(const Warrior& enemy) const
{
    return m_Power > enemy.m_Power;
}


