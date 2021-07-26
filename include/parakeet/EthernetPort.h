/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_ETHERNETPORT_H
#define PARAKEET_ETHERNETPORT_H

#include <string> 
#include <cstring>

#if defined(_WIN32)
	#include <WS2tcpip.h>
	#include <winsock2.h>

	#pragma comment(lib, "ws2_32.lib")
#elif defined(__linux) || defined(linux) || defined(__linux__)
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <unistd.h>
#endif

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

	#if defined(_WIN32)
		WSADATA wsaData;
	#endif
};
}
}

#endif