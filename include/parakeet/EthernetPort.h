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
	EthernetPort() = default;

	/// \brief Open an ethernet port for IO communication
	/// \param[in] ipAddress - The IP Address of the device to communicate with
	/// \param[in] lidarPort - The port that the device is sending to us on
	/// \param[in] localPort - The port which this device will send out messages from
	/// \returns If opening the port was successful
	bool open(const char* ipAddress, int lidarPort, int localPort);

	/// \brief Close an ethernet port to stop IO communication
	void close();

	/// \brief Read data from an open ethernet port
	/// \param[in] buffer - The buffer in-which read data will be placed
	/// \param[in] currentLength - The number of bytes used in the current buffer
	/// \param[in] bufferSize - The maximum size of the buffer
	/// \returns The number of bytes read from the stream
	int read(unsigned char* buffer, int currentLength, int bufferSize);

	/// \brief Write data to an open ethernet port
	/// \param[in] message - The message which will be sent
	void write(const std::string& message);

	/// \brief Send a message across the communication lines and await a specific response, timeout if the response takes too long
	/// \param[in] message - The message to send out
	/// \param[in] response - The response we are awaiting
	/// \param[in] timeout - How long in milliseconds until this function should be forced to return
	/// \returns True if the response message was received
	bool sendMessageWaitForResponseOrTimeout(const std::string& message, const std::string& response, std::chrono::milliseconds timeout);

	/// \returns The current connection state
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