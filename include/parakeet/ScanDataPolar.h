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
        /// \returns A ScanDataPolar object which holds the information given through the param, and generates a timestamp.
        ScanDataPolar(const std::vector<PointPolar>& pointPolarList);
        
        /// \brief Returns the vector of points this object is holding onto
        /// \returns Vector of points this object is holding onto
        const std::vector<PointPolar>& getPoints() const;
        
        /// \brief Returns the timestamp of this objects creation
        /// \returns Timestamp of this objects creation
        const std::chrono::time_point<std::chrono::system_clock>& getTimestamp() const;

    private:
        std::vector<PointPolar> points_mm;
        std::chrono::time_point<std::chrono::system_clock> timestamp;
};
}
}