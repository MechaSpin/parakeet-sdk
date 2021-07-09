/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/internal/SensorResponse.h>

namespace mechaspin
{
namespace parakeet
{
namespace internal
{
    SensorResponse::SensorResponse(MessageType messageType, const std::string& message)
    {
        this->possibleResponses.push_back(message);
        this->messageType = messageType;
    }

    SensorResponse::SensorResponse(MessageType messageType, const std::vector<std::string>& messages)
    {
        for(auto message : messages)
        {
            this->possibleResponses.push_back(message);
        }
        this->messageType = messageType;
    }
    
    SensorResponse::MessageType SensorResponse::getMessageType() const
    {
        return this->messageType;
    }
    
    const std::vector<std::string>& SensorResponse::getResponses() const
    {
        return this->possibleResponses;
    }
}
}
}