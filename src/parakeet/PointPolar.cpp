/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/PointPolar.h>

namespace mechaspin
{
namespace parakeet
{
    PointPolar::PointPolar(double range, double angle_deg, std::uint16_t intensity)
    {
        this->range = range;
        this->angle_deg = angle_deg;
        this->intensity = intensity;
    }

    double PointPolar::getRange() const
    {
        return range;
    }

    double PointPolar::getAngleInDegrees() const
    {
        return angle_deg;
    }

    std::uint16_t PointPolar::getIntensity() const
    {
        return intensity;
    }
}
}