/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_ETHERNETPORT_H
#define PARAKEET_ETHERNETPORT_H

#include <string> 
#include <cstring>
#include <thread>
#include <chrono>

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

	bool sendMessageWaitForResponseOrTimeout(const std::string& message, const std::string& response, std::chrono::milliseconds timeout);

	bool isConnected();
private:
	struct EthernetConnection
	{
		int socket = 0;
		std::string ipAddress;
		int lidarPort;
		int localPort;
	};

	unsigned int stm32crc(unsigned int* ptr, unsigned int len);

	EthernetConnection ethernetConnection;
};
}
}

#endif