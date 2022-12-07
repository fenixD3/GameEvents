#include "MapPoint.h"

MapPoint::MapPoint(int x, int y)
    : m_X(x)
    , m_Y(y)
{}

MapPoint::operator std::string() const
{
    return boost::lexical_cast<std::string>(m_X) + ' ' + boost::lexical_cast<std::string>(m_Y);
}

std::ostream& operator<<(std::ostream& out, const MapPoint& point)
{
    out << boost::lexical_cast<std::string>(point.m_X) + ' ' + boost::lexical_cast<std::string>(point.m_Y);
    return out;
}

int MapPoint::GetXCoord() const
{
    return m_X;
}

int MapPoint::GetYCoord() const
{
    return m_Y;
}
