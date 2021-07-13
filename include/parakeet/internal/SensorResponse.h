/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_SENSORRESPONSE_H
#define PARAKEET_SENSORRESPONSE_H

#include <string>
#include <vector>

namespace mechaspin
{
namespace parakeet
{
namespace internal
{
class SensorResponse
{
    public:
        enum MessageType
        {
            STOP,
            START,
            INTENSITY,
            SPEED,
            BAUDRATE,
            DATASMOOTHING,
            DRAGPOINTREMOVAL,
            NA
        };

        SensorResponse(MessageType messageType, const std::string& message);
        SensorResponse(MessageType messageType, const std::vector<std::string>& messages);

        MessageType getMessageType() const;

        const std::vector<std::string>& getResponses() const;
    private:
        std::vector<std::string> possibleResponses;
        MessageType messageType;
};
}
}
}

#endif