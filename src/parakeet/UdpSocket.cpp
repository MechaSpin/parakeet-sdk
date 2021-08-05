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
	const int MAX_BUFFER_LENGTH = 8192;
	const int MAX_IP_LENGTH = 200;

	#if defined(_WIN32)
		WSADATA wsaData;
	#endif

	bool UdpSocket::open(int srcPort)
	{
		#if defined(_WIN32)
			WSAStartup(MAKEWORD(2, 2), &wsaData);
		#endif

		socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (socket == -1)
		{
			return false;
		}

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(srcPort);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (::bind(socket, (struct sockaddr*)&addr, sizeof(addr)) != 0)
		{
			return false;
		}

		return true;
	}

	void UdpSocket::close()
	{
		if (isConnected())
		{
			#if defined(_WIN32)
				closesocket(socket);
				WSACleanup();
			#elif defined(__linux) || defined(linux) || defined(__linux__)
				::close(socket);
			#endif
			socket = 0;
		}
	}

	int UdpSocket::read(const mechaspin::parakeet::internal::BufferData& bufferData, int bufferMaxSize)
	{
		if (isConnected())
		{
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(socket, &fds);

			struct timeval to = { 1, 0 };
			int ret = select(static_cast<int>(socket) + 1, &fds, NULL, NULL, &to);

			if (ret > 0 && FD_ISSET(socket, &fds))
			{
				sockaddr_in addr;

				#if defined(_WIN32)
					int size = sizeof(addr);
				#elif defined(__linux) || defined(linux) || defined(__linux__)
					socklen_t size = sizeof(addr);
				#endif

				int charsRead = recvfrom(socket, 
					(char*)bufferData.buffer + bufferData.length, 
					bufferMaxSize - bufferData.length, 0,
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

	void UdpSocket::write(const mechaspin::parakeet::internal::InetAddress& destinationAddress, const mechaspin::parakeet::internal::BufferData& bufferData)
	{
		sockaddr_in to;
		to.sin_family = AF_INET;
		inet_pton(AF_INET, destinationAddress.ipAddress.c_str(), &to.sin_addr.s_addr);

		to.sin_port = htons(destinationAddress.port);

		sendto(socket, (char*)bufferData.buffer, bufferData.length , 0, (struct sockaddr*)&to, sizeof(struct sockaddr));
	}

	bool UdpSocket::sendMessageWaitForResponseOrTimeout(const mechaspin::parakeet::internal::InetAddress& destinationAddress, 
		const mechaspin::parakeet::internal::BufferData& bufferData, const std::string& response, std::chrono::milliseconds timeout)
	{
		if (!isConnected())
		{
			return false;
		}

		auto startTime = std::chrono::system_clock::now();

		write(destinationAddress, bufferData);

		long long secondsPast = 0;

		while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count() < timeout.count())
		{
			auto totalSecondsPast = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - startTime).count();
			if (totalSecondsPast != secondsPast)
			{
				secondsPast = totalSecondsPast;

				write(destinationAddress, bufferData);
			}

			unsigned char buffer[MAX_BUFFER_LENGTH];
			int charsRead = read(mechaspin::parakeet::internal::BufferData(buffer, 0), MAX_BUFFER_LENGTH);

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
		return socket != 0;
	}
}
}
