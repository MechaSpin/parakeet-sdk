/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/UdpSocket.h>

#include <iostream>

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
	const int MAX_IP_LENGTH = 200;

	#if defined(_WIN32)
		WSADATA wsaData;
	#endif

	bool UdpSocket::open(const char* ipAddress, int srcPort)
	{
		#if defined(_WIN32)
			WSAStartup(MAKEWORD(2, 2), &wsaData);
		#endif

		ethernetConnection.socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (ethernetConnection.socket == -1)
		{
			return false;
		}

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(srcPort);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (::bind(ethernetConnection.socket, (struct sockaddr*)&addr, sizeof(addr)) != 0)
		{
			return false;
		}

		ethernetConnection.srcPort = srcPort;
		ethernetConnection.ipAddress = std::string(ipAddress);

		return true;
	}

	void UdpSocket::close()
	{
		if (isConnected())
		{
			#if defined(_WIN32)
				closesocket(ethernetConnection.socket);
				WSACleanup();
			#elif defined(__linux) || defined(linux) || defined(__linux__)
				::close(ethernetConnection.socket);
			#endif
			ethernetConnection.socket = 0;
		}
	}

	int UdpSocket::read(unsigned char* buffer, int currentLength, int bufferSize)
	{
		if (isConnected())
		{
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(ethernetConnection.socket, &fds);

			struct timeval to = { 1, 0 };
			int ret = select(static_cast<int>(ethernetConnection.socket) + 1, &fds, NULL, NULL, &to);

			if (ret > 0 && FD_ISSET(ethernetConnection.socket, &fds))
			{
				sockaddr_in addr;

				
				#if defined(_WIN32)
					int size = sizeof(addr);
				#elif defined(__linux) || defined(linux) || defined(__linux__)
					socklen_t size = sizeof(addr);
				#endif

				int charsRead = recvfrom(ethernetConnection.socket, 
					(char*)buffer + currentLength, 
					bufferSize - currentLength, 0, 
					(struct sockaddr*)&addr, &size);

				if (charsRead == -1)
				{
					/*
					#if defined(_WIN32)
						auto lastError = WSAGetLastError();
                        std::cout << "Oh dear, something went wrong with read()! " << lastError << strerror(lastError) << std::endl;
					#elif defined(__linux) || defined(linux) || defined(__linux__)
						std::cout << "Oh dear, something went wrong with read()!" << strerror(errno) << std::endl;
					#endif
					*/
					return 0;
				}
				else
				{
					return charsRead;
				}
			}
		}

		return 0;
	}

	void UdpSocket::write(const char* ipAddress, unsigned short dstPort, const char* buffer, unsigned int length)
	{
		sockaddr_in to;
		to.sin_family = AF_INET;
		inet_pton(AF_INET, ipAddress, &to.sin_addr.s_addr);

		to.sin_port = htons(dstPort);

		sendto(ethernetConnection.socket, buffer, length, 0, (struct sockaddr*)&to, sizeof(struct sockaddr));
	}

	bool UdpSocket::sendMessageWaitForResponseOrTimeout(const char* ipAddress, unsigned short dstPort, const char* buffer, unsigned int length, const std::string& response, std::chrono::milliseconds timeout)
	{
		if (!isConnected())
		{
			return false;
		}

		auto startTime = std::chrono::system_clock::now();

		write(ipAddress, dstPort, buffer, length);

		long long secondsPast = 0;

		while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count() < timeout.count())
		{
			auto totalSecondsPast = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - startTime).count();
			if (totalSecondsPast != secondsPast)
			{
				secondsPast = totalSecondsPast;

				write(ipAddress, dstPort, buffer, length);
			}

			unsigned char buffer[1000];
			int charsRead = read(buffer, 0, 1000);

			if (charsRead != 0)
			{
				std::string stringForm((char*)buffer, charsRead);
				if (stringForm.rfind(response) != std::string::npos)
				{
					return true;
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		return false;
	}

	bool UdpSocket::isConnected()
	{
		return ethernetConnection.socket != 0;
	}
}
}
