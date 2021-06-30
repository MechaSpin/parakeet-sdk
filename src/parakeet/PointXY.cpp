/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/PointXY.h>

namespace mechaspin
{
namespace parakeet
{
    PointXY::PointXY(double x_mm, double y_mm, std::uint16_t intensity)
    {
        this->x_mm = x_mm;
        this->y_mm = y_mm;
        this->intensity = intensity;
    }

    double PointXY::getX() const
    {
        return getX_mm();
    }

    double PointXY::getY() const
    {
        return getY_mm();
    }

    std::uint16_t PointXY::getIntensity() const
    {
        return intensity;
    }
}
}