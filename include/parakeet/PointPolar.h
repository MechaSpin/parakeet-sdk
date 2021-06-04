/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#pragma once

#include <cstdint>

namespace mechaspin
{
namespace parakeet
{
class PointPolar
{
    public:
        /// \brief Create a PointPolar which is responsible for holding on to a polar coordinate.
        /// \param[in] range - Distance from the origin
        /// \param[in] angle_deg - Polar angle in degree form
        /// \param[in] intensity - The intensity value of the point
        /// \returns A PointPolar object which holds the information given through the params.
        PointPolar(double range, double angle_deg, std::uint16_t intensity);

        /// \brief Returns the distance from the origin
        /// \returns Distance from the origin
        double getRange() const;

        /// \brief Returns the polar angle in degree form
        /// \returns Polar angle in degree form
        double getAngleInDegrees() const;

        /// \brief Returns the intensity value of the point
        /// \returns Intensity value of the point
        std::uint16_t getIntensity() const;
    private:
        double range;
        double angle_deg;
        std::uint16_t intensity;
};
}
}