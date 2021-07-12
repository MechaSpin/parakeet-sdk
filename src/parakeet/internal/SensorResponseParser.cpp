/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/internal/SensorResponseParser.h>

namespace mechaspin
{
namespace parakeet
{
namespace internal
{
    SensorResponseParser::SensorResponseParser()
    {
        SensorResponse baudRate(SensorResponse::BAUDRATE, "Error: OK");
        SensorResponse dataSmoothing(SensorResponse::DATASMOOTHING, "LiDAR set smooth ok");
        SensorResponse dragPointRemoval(SensorResponse::DRAGPOINTREMOVAL, "LiDAR set filter ok");
        SensorResponse intensity(SensorResponse::INTENSITY, std::vector<std::string> { "LiDAR CONFID", "LiDAR NO CONFID" });
        SensorResponse speed(SensorResponse::SPEED, "Set RPM: OK");
        SensorResponse start(SensorResponse::START, "LiDAR START");
        SensorResponse stop(SensorResponse::STOP, "LiDAR STOP");

        allResponses = 
        {
            baudRate,
            dataSmoothing,
            dragPointRemoval,
            intensity,
            speed,
            start,
            stop
        };
    }

    SensorResponse SensorResponseParser::getSensorResponseFromMessage(const std::string& message)
    {
        for(SensorResponse response : allResponses)
        {
            if(doesMessageMatchResponse(message, response))
            {
                return response;
            }
        }
        return SensorResponse(SensorResponse::NA, "");
    }

    bool SensorResponseParser::doesMessageMatchResponse(const std::string& message, const SensorResponse& response)
    {
        for(auto responseMessage : response.getResponses())
        {
            if(message.find(responseMessage) != std::string::npos)
            {
                return true;
            }
        }
        return false;
    }
}
}
}