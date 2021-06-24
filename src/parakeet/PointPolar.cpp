/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/PointPolar.h>

namespace mechaspin
{
namespace parakeet
{
    PointPolar::PointPolar(double range_mm, double angle_deg, std::uint16_t intensity)
    {
        this->range_mm = range_mm;
        this->angle_deg = angle_deg;
        this->intensity = intensity;
    }

    double PointPolar::getRange() const
    {
        return getRange_mm();
    }

    double PointPolar::getRange_mm() const
    {
        return range_mm;
    }

    double PointPolar::getAngleInDegrees() const
    {
        return getAngle_deg();
    }

    double PointPolar::getAngle_deg() const
    {
        return angle_deg;
    }

    std::uint16_t PointPolar::getIntensity() const
    {
        return intensity;
    }
}
}