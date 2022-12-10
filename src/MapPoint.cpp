#include "MapPoint.h"

#include <cmath>

MapPoint::MapPoint(int x, int y)
    : m_X(x)
    , m_Y(y)
{}

int MapPoint::GetXCoord() const
{
    return m_X;
}

int MapPoint::GetYCoord() const
{
    return m_Y;
}

double MapPoint::LengthTo(MapPoint dest) const
{
    return std::hypot(dest.m_X - m_X, dest.m_Y - m_Y);
}

MapPoint::operator std::string() const
{
    return boost::lexical_cast<std::string>(m_X) + ' ' + boost::lexical_cast<std::string>(m_Y);
}

bool operator==(const MapPoint lhs, const MapPoint rhs)
{
    return lhs.m_X == rhs.m_X && lhs.m_Y == rhs.m_Y;
}

bool operator!=(const MapPoint lhs, const MapPoint rhs)
{
    return !(lhs == rhs);
}

//bool operator<(const MapPoint lhs, const MapPoint rhs)
//{
//    return lhs.m_X < rhs.m_X && lhs.m_Y < rhs.m_Y;
//}
//
//bool operator>(const MapPoint lhs, const MapPoint rhs)
//{
//    return rhs < lhs;
//}
//
//bool operator<=(const MapPoint lhs, const MapPoint rhs)
//{
//    return !(rhs < lhs);
//}
//
//bool operator>=(const MapPoint lhs, const MapPoint rhs)
//{
//    return !(lhs < rhs);
//}

std::ostream& operator<<(std::ostream& out, const MapPoint point)
{
    out << point.m_X << ' ' << point.m_Y;
    return out;
}

size_t MapPointHasher::operator()(const MapPoint point) const
{
    return std::hash<decltype(point.GetXCoord())>()(point.GetXCoord()) + std::hash<decltype(point.GetYCoord())>()(point.GetYCoord());
}
