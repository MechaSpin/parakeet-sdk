/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_ETHERNETPORT_H
#define PARAKEET_ETHERNETPORT_H

#include <string> 
#include <cstring>
#include <thread>
#include <chrono>

#include <parakeet/internal/BufferData.h>
#include <parakeet/internal/InetAddress.h>

namespace mechaspin
{
namespace parakeet
{
class UdpSocket
{
public:
	UdpSocket() = default;

	/// \brief Open a UDP Socket for reading
	/// \param[in] srcPort - The port which this device will read messages from
	/// \returns If opening the port was successful
	bool open(int srcPort);

	/// \brief Close the existing UDP Socket
	void close();

	/// \brief Read data from an opened UDP socket
	/// \param[in] bufferData - The buffer in-which read data will be placed
	/// \param[in] bufferMaxSize - The maximum size of the buffer
	/// \returns The number of bytes read from the stream
	int read(const mechaspin::parakeet::internal::BufferData& bufferData, int bufferMaxSize);

	/// \brief Write data to a specific destination
	/// \param[in] destinationAddress - The destination address of the device to communicate with
	/// \param[in] bufferData - The data which will be sent
	void write(const mechaspin::parakeet::internal::InetAddress& destinationAddress, const mechaspin::parakeet::internal::BufferData& bufferData);

	/// \brief Send a message across the communication lines and await a specific response, timeout if the response takes too long
	/// \param[in] destinationAddress - The destination address of the device to communicate with
	/// \param[in] bufferData - The data which will be sent
	/// \param[in] response - The response we are awaiting
	/// \param[in] timeout - How long in milliseconds until this function should be forced to return
	/// \returns True if the response message was received
	bool sendMessageWaitForResponseOrTimeout(const mechaspin::parakeet::internal::InetAddress& destinationAddress, const mechaspin::parakeet::internal::BufferData& bufferData, const std::string& response, std::chrono::milliseconds timeout);

	/// \returns The current connection state
	bool isConnected();
private:

	#if defined(_WIN32)
		unsigned long long socket = 0;
	#elif defined(__linux) || defined(linux) || defined(__linux__)
		int socket = 0;
	#endif
};
}
}

#endif