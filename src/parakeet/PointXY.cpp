/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/PointXY.h>

namespace mechaspin
{
namespace parakeet
{
    PointXY::PointXY(double x, double y, std::uint16_t intensity)
    {
        this->x = x;
        this->y = y;
        this->intensity = intensity;
    }

    double PointXY::getX() const
    {
        return x;
    }

    double PointXY::getY() const
    {
        return y;
    }

    std::uint16_t PointXY::getIntensity() const
    {
        return intensity;
    }
}
}