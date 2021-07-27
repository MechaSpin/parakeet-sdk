/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_ETHERNETPORT_H
#define PARAKEET_ETHERNETPORT_H

#include <string> 
#include <cstring>

namespace mechaspin
{
namespace parakeet
{
class EthernetPort
{
public:
	EthernetPort();

	bool open(const char* ipAddress, int lidarPort, int localPort);
	void close();

	int read(unsigned char* buffer, int currentLength, int bufferSize);
	void write(const std::string& message);

	bool isConnected();
private:
	struct EthernetConnection
	{
		int socket = 0;
		std::string ipAddress;
		int lidarPort;
		int localPort;
	};

	EthernetConnection ethernetConnection;
};
}
}

#endif