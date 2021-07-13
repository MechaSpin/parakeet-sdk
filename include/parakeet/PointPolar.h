/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_POINTPOLAR_H
#define PARAKEET_POINTPOLAR_H

#include <cstdint>
#include "macros.h"

namespace mechaspin
{
namespace parakeet
{
class PointPolar
{
    public:
        /// \brief Create a PointPolar which is responsible for holding on to a polar coordinate.
        /// \param[in] range_mm - Distance, in millimeters, from the origin
        /// \param[in] angle_deg - Polar angle in degree form
        /// \param[in] intensity - The intensity value of the point
        /// \returns A PointPolar object which holds the information given through the params.
        PointPolar(double range_mm, double angle_deg, std::uint16_t intensity);

        /// \deprecated Replaced by getRange_mm()
        /// \brief Returns the distance, in millimeters, from the origin
        /// \returns Distance, in millimeters, from the origin
        PARAKEET_DEPRECATED(double getRange() const);

        /// \brief Returns the distance, in millimeters, from the origin
        /// \returns Distance, in millimeters, from the origin
        double getRange_mm() const;

        /// \deprecated Replaced by getAngle_deg()
        /// \brief Returns the polar angle in degree form
        /// \returns Polar angle in degree form
        PARAKEET_DEPRECATED(double getAngleInDegrees() const);

        /// \brief Returns the polar angle in degree form
        /// \returns Polar angle in degree form
        double getAngle_deg() const;

        /// \brief Returns the intensity value of the point
        /// \returns Intensity value of the point
        std::uint16_t getIntensity() const;
    private:
        double range_mm;
        double angle_deg;
        std::uint16_t intensity;
};
}
}

#endif