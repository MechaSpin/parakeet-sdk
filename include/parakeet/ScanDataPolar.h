/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#pragma once

#include "PointPolar.h"

#include <vector>
#include <chrono>

namespace mechaspin
{
namespace parakeet
{
class ScanDataPolar
{
    public:
        /// \brief Create a ScanDataPolar which is responsible for holding on to a list of PointPolars
        /// \param[in] pointPolarList - A vector containing PointPolar(s)
        /// \param[in] timestamp - A time point which holds the time the first point was received
        /// \returns A ScanDataPolar object which holds a vector of PointPolar(s) and a timestamp
        ScanDataPolar(const std::vector<PointPolar>& pointPolarList, const std::chrono::time_point<std::chrono::system_clock>& timestamp);
        
        /// \brief Returns the vector of points this object is holding onto
        const std::vector<PointPolar>& getPoints() const;
        
        /// \brief Returns the timestamp which signals when the first point was received
        const std::chrono::time_point<std::chrono::system_clock>& getTimestamp() const;

    private:
        std::vector<PointPolar> points_mm;
        std::chrono::time_point<std::chrono::system_clock> timestamp;
};
}
}