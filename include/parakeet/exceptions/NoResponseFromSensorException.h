/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_NORESPONSEFROMSENSOREXCEPTION_H
#define PARAKEET_NORESPONSEFROMSENSOREXCEPTION_H

#include <stdexcept>

namespace mechaspin
{
    namespace parakeet
    {
        namespace exceptions
        {
            class NoResponseFromSensorException : public std::runtime_error
            {
            public:
                NoResponseFromSensorException();
            };
        }
    }
}

#endif