/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_SCANDATAPOLAR_H
#define PARAKEET_SCANDATAPOLAR_H

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
        /// \param[in] vectorOfPolarPoints - A vector containing PointPolar(s)
        /// \param[in] timestampOfFirstPoint - A time point which holds the time the first point was received
        /// \returns A ScanDataPolar object which holds a vector of PointPolar(s) and a timestamp
        ScanDataPolar(const std::vector<PointPolar>& vectorOfPolarPoints, const std::chrono::time_point<std::chrono::system_clock>& timestampOfFirstPoint);
        
        /// \brief Returns the vector of points this object is holding onto
        const std::vector<PointPolar>& getPoints() const;
        
        /// \brief Returns the timestamp which signals when the first point was received
        const std::chrono::time_point<std::chrono::system_clock>& getTimestamp() const;

    private:
        std::vector<PointPolar> vectorOfPolarPoints;
        std::chrono::time_point<std::chrono::system_clock> timestampOfFirstPoint;
};
}
}

#endif