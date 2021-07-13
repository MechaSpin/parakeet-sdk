/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_SENSORRESPONSEPARSER_H
#define PARAKEET_SENSORRESPONSEPARSER_H

#include <parakeet/internal/SensorResponse.h>

namespace mechaspin
{
namespace parakeet
{
namespace internal
{
class SensorResponseParser
{
    public:
        SensorResponseParser();
        SensorResponse getSensorResponseFromMessage(const std::string& message);

    private:
        bool doesMessageMatchResponse(const std::string& message, const SensorResponse& response);

        std::vector<SensorResponse> allResponses;
};
}
}
}

#endif