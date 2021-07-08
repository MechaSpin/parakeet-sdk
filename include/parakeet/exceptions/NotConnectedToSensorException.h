/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_NOTCONNECTEDTOSENSOREXCEPTION_H
#define PARAKEET_NOTCONNECTEDTOSENSOREXCEPTION_H

#include <stdexcept>

namespace mechaspin
{
namespace parakeet
{
namespace exceptions
{
class NotConnectedToSensorException : public std::runtime_error
{
    public:
        NotConnectedToSensorException();
};
}
}
}

#endif