/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#pragma once

#include <cstdint>

namespace mechaspin
{
namespace parakeet
{
class PointXY
{
    public:
        /// \brief Create a PointXY which is responsible for holding on to a cartesian coordinate.
        /// \param[in] x - Distance from the origin in the X direction
        /// \param[in] y - Distance from the origin in the Y direction
        /// \param[in] intensity - The intensity value of the point
        /// \returns A PointXY object which holds the information given through the params.
        PointXY(double x, double y, std::uint16_t intensity);

        /// \brief Returns the distance from the origin in the X direction
        /// \returns Distance from the origin in the X direction
        double getX() const;

        /// \brief Returns the distance from the origin in the Y direction
        /// \returns Distance from the origin in the Y direction
        double getY() const;

        /// \brief Returns the intensity value of the point
        /// \returns Intensity value of the point
        std::uint16_t getIntensity() const;

    private:
        double x;
        double y;
        std::uint16_t intensity;
};
}
}