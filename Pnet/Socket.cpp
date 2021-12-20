#include "Socket.h"

#include <assert.h>
#include <iostream>
#include <Windows.h>
#include <lmon.h>
#include <stdlib.h>

namespace Pnet
{
	Socket::Socket(PVersion ipversion, SocketHandle handle)
		:ipversion(ipversion), handle(handle)
	{
		assert(ipversion == PVersion::IPv4);
	}



	PResult Socket::create()
	{
		assert(ipversion == PVersion::IPv4);
		if (handle != INVALID_SOCKET)
		{
			return PResult::P_NotYetImplemented;
		}

		handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (handle == INVALID_SOCKET)
		{
			int error = WSAGetLastError();

			return PResult::P_NotYetImplemented;
		}
		if (SetSocketOption(SocketOption::TCP_NoDelay, TRUE) != P_Success)
		{
			return PResult::P_NotYetImplemented;
		}



		return PResult::P_Success;
	}


	PResult Socket::close()
	{
		if (handle == INVALID_SOCKET)
		{
			return PResult::P_NotYetImplemented;
		}

		int result = closesocket(handle);

		if (result != 0)
		{
			int error = WSAGetLastError();
			std::cout << "socket is closed or not yet implemented" << std::endl;
			return PResult::P_NotYetImplemented;
		}
		handle = INVALID_SOCKET;
		return PResult::P_Success;
	}
	
	SocketHandle Socket::GetHandle()
	{
		return handle;
	}
	void Socket::SetHandle(const SocketHandle hdl)
	{
		handle = hdl;
	}
	void Socket::AppendBufffer(const char buf)
	{
		sockbuf.append(&buf);
	}

	void Socket::Clearbuffer()
	{
		sockbuf.clear();
	}

	void Socket::SetBuffer(std::string buf)
	{
		sockbuf = buf;
	}
	std::string Socket::GetSenderName()
	{
		return senderName;
	}
	void Socket::GetHostname()
	{
		wchar_t* hostStr;
		ULONG hostStr_len;
		//GetHostNameW(hostStr, hostStr_len);

				
	}
	PVersion Socket::GetIPVersion()
	{
		return ipversion;
	}

	std::string Socket::GetBufData()
	{
		return sockbuf;
	}

	size_t Socket::GetSockBufLen()
	{
		return sockbuf.size();
	}



	PResult Socket::Bind(IPEndPoint endpoint)
	{
		sockaddr_in addr = endpoint.GetAddrInfoIPv4();
		
		int result = bind(handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
		if (result != 0)
		{
			int err = WSAGetLastError();
			std::cerr << "failed to bind socket" << std::endl;
			return PResult::P_NotYetImplemented;
		}

		return PResult::P_Success;
	}

	PResult Socket::Listen(IPEndPoint endpoint, int backlog)
	{
		if (Bind(endpoint) == PResult::P_NotYetImplemented)
		{
			std::cout << "failed to bind socket"<<std::endl;
			return PResult::P_NotYetImplemented;
		}

		// Change the socket mode on the listening socket from blocking to

		// non-block so the application will not block waiting for requests

		NonBlock = 1;

		if (ioctlsocket(handle, FIONBIO, &NonBlock) == SOCKET_ERROR)

		{

			printf("ioctlsocket() failed with error %d\n", WSAGetLastError());

			return PResult::P_NotYetImplemented;

		}


			printf("ioctlsocket() is OK!\n");

		int result=listen(handle, backlog);
		if (result != 0)
		{
			std::cout << "socket failed to listen" << std::endl;
			return PResult::P_NotYetImplemented;
		}

		return PResult::P_Success;
	}

	PResult Socket::SetIPEndPoint(Socket &outhandle)
	{
		sockaddr_in connected_addrs = {};
		int len = sizeof(sockaddr_in);
		SocketHandle acceptedhandle = accept(handle,(sockaddr*)&connected_addrs,&len);
		if (acceptedhandle == INVALID_SOCKET)
		{
			std::cout << "failed to accept socket" << handle << std::endl;
			int err = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}
		else
		{

			IPEndPoint new_server_endpoint((sockaddr*)(&connected_addrs));
			std::cout << "connection accepted" << std::endl;
			new_server_endpoint.Print();
			outhandle = Socket(PVersion::IPv4, acceptedhandle);
			return PResult::P_Success;
		}
		
	}

	PResult Socket::Connect(IPEndPoint endpoint)
	{
		
		sockaddr_in addr4 = endpoint.GetAddrInfoIPv4();
		
		int result = connect(handle, (sockaddr*)&addr4, sizeof(sockaddr_in));
		if (result == 0)
		{
			std::cout << "socket connected successfully to sock=" <<(int)handle<< std::endl;
			endpoint.Print();
			return PResult::P_Success;
		}
		else
		{
			int err = WSAGetLastError();
			std::cout << "failed connect to port "<<endpoint.GetPort ()<< std::endl;
				endpoint.Print();
			return PResult::P_NotYetImplemented;
			
		}

	}

	PResult Socket::Send(char * buffer, int buf_size, int& byte_send)
	{
		byte_send= send(handle,(const char *)buffer,buf_size,NULL);
		if (byte_send == 0)
		{
			std::cout << "connection was closed" << std::endl;
			return PResult::P_NotYetImplemented;
		}
		if (byte_send == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			std::cout << "socket erro" << std:: endl;
			return PResult::P_NotYetImplemented;
		}

		return PResult::P_Success;
	}

	PResult Socket::Recv(char * buffer, int buf_size, int& byte_rcved)
	{
	
		byte_rcved = recv(handle, buffer, buf_size, 0);
		
		if (byte_rcved == 0)
		{
			std::cout << "connection was lost" << std::endl;
			return PResult::P_NotYetImplemented;
		}
		if (byte_rcved == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}
		return PResult::P_Success;
	}

	

	

	PResult Socket::Sendall(void* data, int datalen)
	{
		int totalByteSend= 0;
	
		while (totalByteSend < datalen)
		{
			int bytesRemaining = datalen - totalByteSend;
			int byte_send = 0;
			char* bufsetof = (char*)data + totalByteSend;

			PResult result = Send(bufsetof, datalen, byte_send);
			if (result == PResult::P_NotYetImplemented)
			{
				return PResult::P_NotYetImplemented;
				break;
			}
			totalByteSend += byte_send;
			
		}

		return PResult::P_Success;
	}

	PResult Socket::Recvall(void* destination, int destination_len)
	{
		int totalByteReceived = 0;
		char* bufferoffset = new char[destination_len];
		while (totalByteReceived < destination_len)
		{
			int bytesRemaining = destination_len - totalByteReceived;
			int bytesReceived = 0;
			 bufferoffset= ((char*)destination + totalByteReceived);
		
			PResult result = Recv(bufferoffset, bytesRemaining, bytesReceived);
			if (result == PResult::P_NotYetImplemented)
			{
				
				return PResult::P_NotYetImplemented;
			}
			totalByteReceived += bytesReceived;

		}
		

		return PResult::P_Success;
	}

	PResult Socket::SWSASend()
	{
		if (sockbuf.size() > 0)
		{
			size_t bufferlen = htonl(sockbuf.size());
				PResult result_SIZE = Sendall(&bufferlen, sizeof(ULONG));

		
			PResult result = Sendall((char *)sockbuf.c_str(), sockbuf.size());
			if (result == PResult::P_NotYetImplemented || result_SIZE == PResult::P_NotYetImplemented)
			{
				printf("send failed\n");
				return PResult::P_NotYetImplemented;
			}
			else
			{
				printf("send was successful\n");
				return PResult::P_Success;
			}
		}
		else
		{
			printf("failed to send.Data_buf is empty \n");
			return PResult::P_NotYetImplemented ;
		}
	}

	PResult Socket::SWSARecv()
	{
		size_t bufferlen = {};
		
		sockbuf.clear();

		PResult result_SIZE = Recvall(&bufferlen, sizeof(ULONG));
		bufferlen = ntohl(bufferlen);
		
		char* buffer = new char[bufferlen + 1];
		
		PResult result = Recvall(buffer, bufferlen);
		
		
		for (size_t i = 0;i < bufferlen ;i++)
		{
			sockbuf.insert(sockbuf.end(),buffer[i]);
		}

		 //sockbuf.assign( buffer);
		delete[] buffer;
		   
		if (result == PResult::P_NotYetImplemented || result_SIZE == PResult::P_NotYetImplemented)
			{
			
				printf("recv failed\n");
				return PResult::P_NotYetImplemented;
			}
		
		else
		{
			printf("recv was successful \n");
			return PResult::P_Success;
		}
	}

	PResult Socket::Send(Packet* packet)
	{
		uint32_t buffer_size =(uint32_t) packet->buffer.size();
		buffer_size = ntohl(buffer_size);
		
		PResult result = Sendall(&buffer_size, sizeof(uint32_t));
		if (result != PResult::P_Success)
		{
			int err = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}
		
		buffer_size = htonl(buffer_size);
		result = Sendall(packet->buffer.data(), buffer_size);
		if (result != PResult::P_Success)
		{
			int err = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}

			return PResult::P_Success;
		
	}

	PResult Socket::Recv(Packet* packet)
	{
		packet->Clear();
		uint32_t buffer_size;
		PResult result = Recvall(&buffer_size, sizeof(uint32_t));
		if (result != PResult::P_Success)
		{
			int err = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}

		buffer_size = ntohl(buffer_size);
		packet->buffer.resize(buffer_size);
		
		result = Recvall(&packet->buffer[0], buffer_size);

		if (result != PResult::P_Success)
		{
			int err = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}
		
		return PResult::P_Success;
	}

	PResult Socket::SetSocketOption(SocketOption option, BOOL value)
	{
		int result;
		//int iresult;
		//int jresult;
		switch (option)
		{
		case SocketOption::TCP_NoDelay:
			
			result = setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, ( char*)&value, sizeof(value));
			// iresult = setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, (char*)&value, sizeof(value));
			// jresult = setsockopt(handle, SOL_SOCKET, SO_OOBINLINE , (char*)&value, sizeof(value));
			break;
		
		default:
			return PResult::P_NotYetImplemented;
		}

		if (result != 0 )
		{
			int erro = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}

		return PResult::P_Success;
	}
}
