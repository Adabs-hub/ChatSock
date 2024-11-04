/*
autho Adabogo Emmnauel

*/

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



	PResult Socket::create(int sock_type, bool broadcast)
	{
		assert(ipversion == PVersion::IPv4);

		if (sock_type == SOCK_STREAM)
		{
			handle = socket(AF_INET, sock_type, 0);
			if (handle == INVALID_SOCKET)
			{
				std::cout << "invalid handle" << std::endl;
				return PResult::ERR;

			}
			if (SetSocketOption(SocketOption::TCP_NoDelay, TRUE) != OK)
			{
				return PResult::ERR;
			}
		}
		else
		{
			handle = socket(AF_INET, sock_type, IPPROTO_UDP);
			if (handle == INVALID_SOCKET)
			{
				std::cout << "invalid handle" << std::endl;
				return PResult::ERR;

			}
		}
	
		if (broadcast)
		{
			if (SetSocketOption(SocketOption::BROADCAST, TRUE) != OK)
			{
				return PResult::ERR;
			}
		}


		return PResult::OK;
	}


	PResult Socket::close()
	{
		if (handle == INVALID_SOCKET)
		{
			return PResult::ERR;
		}

		int result = closesocket(handle);

		if (result != 0)
		{
			int error = WSAGetLastError();
			std::cout << "socket is closed or not yet implemented" << std::endl;
			return PResult::ERR;
		}
		handle = INVALID_SOCKET;
		return PResult::OK;
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


	std::string Socket::GetHostname() //this functin return the name of the running host
	{
		/*
		wchar_t* hostStr=new wchar_t[hostStr_len];
		GetHostNameW(hostStr, hostStr_len);
		*/
		int len = 32;
		char* hoststr=new char[len];
		gethostname(hoststr, len);
		std::string hostName;
		std::cout << hostName;
		return hostName.assign(hoststr);
			
	}



	PVersion Socket::GetIPVersion()
	{
		return ipversion;
	}


	std::string Socket::GetSocketData()
	{
		return sockbuf;
	}


	size_t Socket::GetSockBufLen()
	{
		return sockbuf.size();
	}


	PResult Socket::Bind(IPEndPoint endpoint)
	{
		myaddr = endpoint.GetAddrInfoIPv4();
		endpoint.Print();
		
		int result = bind(handle, (struct sockaddr*)&myaddr, sizeof(struct sockaddr_in));
		if (result != 0)
		{
			int err = WSAGetLastError();
			std::cerr << "failed to bind socket" << std::endl;
			return PResult::ERR;
		}
		return PResult::OK;
	}



	PResult Socket::Listen(IPEndPoint endpoint, int backlog)
	{
		if (Bind(endpoint) == PResult::ERR)
		{
			std::cout << "failed to bind socket" << std::endl;
			return PResult::ERR;
		}

		NonBlock = 1;

		if (ioctlsocket(handle, FIONBIO, &NonBlock) == SOCKET_ERROR) // Change the socket mode on the listening socket from blocking to non-block so the application will not block waiting for requests
		{

			printf("ioctlsocket() failed with error %d\n", WSAGetLastError());

			return PResult::ERR;

		}

		printf("ioctlsocket() is OK!\n");

		int result = listen(handle, backlog);
		if (result != 0)
		{
			std::cout << "socket failed to listen" << std::endl;
			return PResult::ERR;
		}

		return PResult::OK;
	}



	PResult Socket::Listener(IPEndPoint endpoint, sockaddr_in *their_addr)
	{
		if (Bind(endpoint) == PResult::ERR)	  //bind ip and handle to port
		{
			std::cout << "failed to bind socket" << std::endl;
			return PResult::ERR;
		}
		int max_bufLen = 20;
		int addr_len = sizeof(struct sockaddr_in);
		char* buffer = new char[max_bufLen];
	
		if ((ByteRecv = recvfrom(handle, buffer, max_bufLen-1 , 0,
			reinterpret_cast<SOCKADDR*> (their_addr),&addr_len))==-1)
		{
			printf("failed to receive udp info\n");
			return PResult::ERR;
		}
		std::string data(buffer);
		std::cout << "DATAT FROM server: " << data << std::endl;

		return PResult::OK;
	}



	PResult Socket::talker(IPEndPoint endpoint)
	{
		myaddr = endpoint.GetAddrInfoIPv4();
		ByteSend = 0;
		std::string buffer = "hello who's there!!";
		
		if ((ByteSend = sendto(handle, buffer.c_str(), buffer.size(), 0,
			reinterpret_cast< SOCKADDR*>(&myaddr), sizeof(sockaddr_in))) == -1)
		{
			printf("failed to send udp info\n");
			return PResult::ERR;
		}
		return PResult::OK;
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
			return PResult::ERR;
		}
		else
		{

			IPEndPoint new_server_endpoint((sockaddr*)(&connected_addrs));
			std::cout << "connection accepted" << std::endl;
			new_server_endpoint.Print();
			outhandle = Socket(PVersion::IPv4, acceptedhandle);
			return PResult::OK;
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
			return PResult::OK;
		}
		else
		{
			int err = WSAGetLastError();
			std::cout << "failed connect to port "<<endpoint.GetPort ()<< std::endl;
				endpoint.Print();
			return PResult::ERR;
			
		}

	}


	PResult Socket::Connect(sockaddr_in *addr)
	{
		IPEndPoint endpoint = IPEndPoint((sockaddr*)addr);
		int result = connect(handle, (sockaddr*)addr, sizeof(sockaddr_in));

		if (result == 0)
		{
			std::cout << "socket connected successfully to sock=" << (int)handle << std::endl;
			endpoint.Print();
			return PResult::OK;
		}
		else
		{
			int err = WSAGetLastError();
			std::cout << "failed connect to port " << endpoint.GetPort() << std::endl;
			endpoint.Print();
			return PResult::ERR;

		}

	}



	PResult Socket::Send(char * buffer, size_t buf_size, size_t& byte_send)
	{
		byte_send= send(handle,(const char *)buffer,(int)buf_size,NULL);
		if (byte_send == 0)
		{
			std::cout << "connection was closed" << std::endl;
			return PResult::ERR;
		}
		if (byte_send == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			std::cout << "socket erro" << std:: endl;
			return PResult::ERR;
		}

		return PResult::OK;
	}

   /*
	PResult Socket::Recv(char * buffer, size_t buf_size, size_t& byte_rcved)
	{
		std::cout << "attempting receive..." << std::endl;
		byte_rcved = recv(handle, buffer, buf_size, 0);
		std::cout << byte_rcved <<buffer<< "\t" << std::endl;
		
		if (byte_rcved == 0)
		{
			std::cout << "connection was lost" << std::endl;
			return PResult::ERR;
		}
		if (byte_rcved == SOCKET_ERROR)
		{
			std::cout << "socket err" << std::endl;
			//int err = WSAGetLastError();
			return PResult::ERR;
		}
		std::cout << "recv complete" << std::endl;
		return PResult::OK;
	}
	
   */

	PResult Socket::Recv(char* buffer, int buf_size, int& byte_rcved)
	{
		byte_rcved = recv(handle, (char*)buffer, buf_size, NULL);
		if (byte_rcved == 0)
		{
			std::cout << "connection was lost" << std::endl;
			return PResult::ERR;
		}
		if (byte_rcved == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			return PResult::ERR;
		}
		return PResult::OK;
	}


	PResult Socket::Sendall(void* data, size_t datalen)
	{
		size_t totalByteSend= 0;
	
		while (totalByteSend < datalen)
		{
			size_t bytesRemaining = datalen - totalByteSend;
			size_t byte_send = 0;
			char* bufsetof = (char*)data + totalByteSend;

			PResult result = Send(bufsetof, datalen, byte_send);
			if (result == PResult::ERR)
			{
				return PResult::ERR;
				break;
			}
			totalByteSend += byte_send;
			
		}

		return PResult::OK;
	}


	PResult Socket::Recvall(void* destination, size_t destination_len)
	{

		int byte_recved = 0;
		int totalrecveddata = 0;
		PResult result = PResult::OK;
		while (totalrecveddata < destination_len)
		{
			char* bufsetof = (char*)destination + totalrecveddata;
			result = Recv(bufsetof, destination_len, byte_recved);
			if (result == PResult::ERR)
			{

				return PResult::ERR
					;
			}
			totalrecveddata += byte_recved;

		}

		return PResult::OK;

		/*
		size_t totalByteReceived = 0;
		char* bufferoffset = new char[destination_len];
		while (totalByteReceived < destination_len)
		{
			size_t bytesRemaining = destination_len - totalByteReceived;
			size_t bytesReceived = 0;
			 bufferoffset= ((char*)destination + totalByteReceived);
		
			PResult result = Recv(bufferoffset, bytesRemaining, bytesReceived);
			if (result == PResult::ERR)
			{				
				delete[] bufferoffset;
				return PResult::ERR;
			}
			totalByteReceived += bytesReceived;

		}
		delete[] bufferoffset;
		return PResult::OK;
		*/
	}


	PResult Socket::SWSASend(std::string text)
	{
		std::string data = text;
		if (data.size() > 0)
		{
			PResult result_SIZE, result;
			size_t bufferlen = {};
			std::string hostName = GetHostname();
			
			bufferlen =(size_t)htonl((u_long)hostName.size());
			result_SIZE = Sendall(&bufferlen, sizeof(size_t)); //send hostName length
			result = Sendall((char*)hostName.c_str(), hostName.size());//send hostName

			if (result == PResult::OK || result_SIZE == PResult::OK)
			{
					 bufferlen =(size_t) htonl((u_long)data.size());
					result_SIZE = Sendall(&bufferlen, sizeof(size_t));
					result = Sendall((char*)data.c_str(), data.size());

					if (result == PResult::OK || result_SIZE == PResult::OK)
					{
						printf("send\n");
						return PResult::OK;
					}
					else
					{
						printf("failed to send data\n");
						return PResult::ERR;
					}
			}
			else
			{
				printf("failed to send hostName\n");
				return PResult::ERR;
			}
		}
		else
		{
			printf("failed to send.Data_buf is empty \n");
			return PResult::ERR ;
		}
	}


PResult Socket::SWSARecv()
	{
		sockbuf.clear();
		senderName.clear();
		 size_t senderNameBuf_len, databufflen = {};
 
		PResult result_SIZE = Recvall(&senderNameBuf_len, sizeof(size_t));
		senderNameBuf_len =(size_t)ntohl((u_long)senderNameBuf_len);
		std::cout << "size of data received==" << senderNameBuf_len << std::endl;
		char* senderNamebuffer = new char[senderNameBuf_len + 1];
		PResult result = Recvall(senderNamebuffer, senderNameBuf_len);


		if (result == PResult::OK || result_SIZE == PResult::OK)
		{		
			result_SIZE = Recvall(&databufflen, sizeof(size_t));
			databufflen =(size_t) ntohl((u_long)databufflen);

			char* databuffer = new char[databufflen + 1];
			result = Recvall(databuffer,databufflen);

			 if (result == PResult::OK || result_SIZE == PResult::OK)
			 {
				 if(senderNameBuf_len>databufflen)
					 for (size_t i = 0; i < senderNameBuf_len; i++)
					 {
						 if (i < databufflen)
								 sockbuf.insert(sockbuf.end(), databuffer[i]);

						 senderName.insert(senderName.end(), senderNamebuffer[i]);
					 }
				 else
					 for (size_t i = 0; i < databufflen; i++)
					 {
						 if (i < senderNameBuf_len)
								 senderName.insert(senderName.end(), senderNamebuffer[i]);

						 sockbuf.insert(sockbuf.end(), databuffer[i]);
					 }
				 delete[] databuffer;
				 delete[] senderNamebuffer;
				 //printf("recv was successful \n");
				 return PResult::OK;
				 
			 }
			 else
			 {
				 printf("fialed to receive data\n");
				 delete[] databuffer;
				 delete[] senderNamebuffer;
				 return PResult::ERR;
			 }
		}
		else
		{
			delete[] senderNamebuffer;
			printf("fialed to receive SenderName\n");
			return PResult::ERR;
		}

	}


	PResult Socket::Send(Packet* packet)
	{
		uint32_t buffer_size =(uint32_t) packet->buffer.size();
		buffer_size = ntohl(buffer_size);
		
		PResult result = Sendall(&buffer_size, sizeof(uint32_t));
		if (result != PResult::OK)
		{
			int err = WSAGetLastError();
			return PResult::ERR;
		}
		
		buffer_size = htonl(buffer_size);
		result = Sendall(packet->buffer.data(), buffer_size);
		if (result != PResult::OK)
		{
			int err = WSAGetLastError();
			return PResult::ERR;
		}

			return PResult::OK;
	}


	PResult Socket::Recv(Packet* packet)
	{
		packet->Clear();
		uint32_t buffer_size;
		PResult result = Recvall(&buffer_size, sizeof(uint32_t));
		if (result != PResult::OK)
		{
			int err = WSAGetLastError();
			return PResult::ERR;
		}

		buffer_size = ntohl(buffer_size);
		packet->buffer.resize(buffer_size);
		
		result = Recvall(&packet->buffer[0], buffer_size);

		if (result != PResult::OK)
		{
			int err = WSAGetLastError();
			return PResult::ERR;
		}
		
		return PResult::OK;
	}


	PResult Socket::SetSocketOption(SocketOption option, BOOL value)
	{

		switch (option)
		{
		case SocketOption::TCP_NoDelay:
			// jresult = setsockopt(handle, SOL_SOCKET, SO_OOBINLINE , (char*)&value, sizeof(value));

			if (setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, (char*)&value, sizeof(value)) != 0)
			{
				int erro = WSAGetLastError();
				return PResult::ERR;
			}

			break;
		case SocketOption::BROADCAST:

			if (setsockopt(handle, SOL_SOCKET, SO_BROADCAST, (char*)&value, sizeof(value)) != 0)
			{
				int erro = WSAGetLastError();
				return PResult::ERR;
			}
			break;
		
		default:
			return PResult::ERR;
		}

		return PResult::OK;
	}
}
