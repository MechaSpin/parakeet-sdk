/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_UNABLETODETERMINEBAUDRATEEXCEPTION_H
#define PARAKEET_UNABLETODETERMINEBAUDRATEEXCEPTION_H

#include <stdexcept>

namespace mechaspin
{
namespace parakeet
{
namespace exceptions
{
class UnableToDetermineBaudRateException : public std::runtime_error
{
    public:
        UnableToDetermineBaudRateException();
};
}
}
}

#endif