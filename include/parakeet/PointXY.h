/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#pragma once

#include <cstdint>
#include "macros.h"

namespace mechaspin
{
namespace parakeet
{
class PointXY
{
    public:
        /// \brief Create a PointXY which is responsible for holding on to a cartesian coordinate.
        /// \param[in] x_mm - Distance, in millimeters, from the origin in the X direction
        /// \param[in] y_mm - Distance, in millimeters, from the origin in the Y direction
        /// \param[in] intensity - The intensity value of the point
        /// \returns A PointXY object which holds the information given through the params.
        PointXY(double x_mm, double y_mm, std::uint16_t intensity);

        /// \deprecated Replaced by getX_mm()
        /// \brief Returns the distance, in millimeters, from the origin in the X direction
        /// \returns Distance, in millimeters, from the origin in the X direction
        PARAKEET_DEPRECATED(double getX() const);

        /// \deprecated Replaced by getY_mm()
        /// \brief Returns the distance, in millimeters, from the origin in the Y direction
        /// \returns Distance, in millimeters, from the origin in the Y direction
        PARAKEET_DEPRECATED(double getY() const);

        /// \brief Returns the distance, in millimeters, from the origin in the X direction
        /// \returns Distance, in millimeters, from the origin in the X direction
        double getX_mm() const;

        /// \brief Returns the distance, in millimeters, from the origin in the Y direction
        /// \returns Distance, in millimeters, from the origin in the Y direction
        double getY_mm() const;

        /// \brief Returns the intensity value of the point
        /// \returns Intensity value of the point
        std::uint16_t getIntensity() const;

    private:
        double x_mm;
        double y_mm;
        std::uint16_t intensity;
};
}
}