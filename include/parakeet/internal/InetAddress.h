/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_INETADDRESS_H
#define PARAKEET_INETADDRESS_H

#include <string>

namespace mechaspin
{
    namespace parakeet
    {
        namespace internal
        {
            struct InetAddress
            {
                std::string ipAddress;
                unsigned short port;

                InetAddress(const std::string& ipAddress, unsigned short port)
                {
                    this->ipAddress = ipAddress;
                    this->port = port;
                }
            };
        }
    }
}

#endif