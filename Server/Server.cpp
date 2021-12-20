
// Server code
#include <Pnet/Pnet.h>
#include <iostream>


using namespace Pnet;


int main()
{
    if (Network::Initialize())
    {
        std::cerr << "winsock api intialized succesfully" << std::endl;

      
        Socket ListeningSocket;
		if (ListeningSocket.create() == PResult::P_Success)
		{
			std::cout << "socket created successfully" << std::endl;

			if (ListeningSocket.Listen(IPEndPoint("127.0.0.1", 9034), 20) == PResult::P_Success)
			{
				std::cout << "socket successfully bind and listen to port 9043" << std::endl;
				Socket connectedsocket;


				// std::string buffer="";
				Packet packet;
				sockaddr_in internetaddr = {};
				SOCKET AcceptedHandle;
				fd_set WriteSet;
				fd_set ReadSet;
				
			

				
				//WSABUF Databuf;
				//BOOL Read = true;
				timeval timeout;
				DWORD i;
				//DWORD SenderArrayIndex;
				//DWORD Total;
				DWORD NonBlock=0;
				Sock_Info_Array SocketArray;

				PResult result = PResult::P_Success;
				while (true)
				{


					/*8888888888888888888888888888888888888888888888888888888888888888888888888888888*/


						// Prepare the Read and Write socket sets for network I/O notification

					FD_ZERO(&WriteSet);

					FD_ZERO(&ReadSet);

					// Always look for connection attempts

					FD_SET(ListeningSocket.GetHandle(), &WriteSet);
					FD_SET(ListeningSocket.GetHandle(), &ReadSet);
						


						timeout.tv_sec = 0;
						timeout.tv_usec = 200;

					if ((select(ListeningSocket.GetHandle()+1, &ReadSet, &WriteSet, NULL, &timeout)) == SOCKET_ERROR)

						{

							printf("select() returned with error %d\n", WSAGetLastError());

							return 1;

						}

						

							//printf("select() is OK!\n");


					
					// Check for arriving connections on the listening socket.

					if (FD_ISSET(ListeningSocket.GetHandle(), &ReadSet))

					{

						sockaddr_in accepted_addrs = {};
						int len = sizeof(sockaddr_in);

						
						if((AcceptedHandle= accept(ListeningSocket.GetHandle(), (sockaddr*)&accepted_addrs, &len)) != INVALID_SOCKET)

						{
							printf("connection gotting from ..");

							

							// Set the accepted socket to non-blocking mode so the server will

							// not get caught in a blocked condition on WSASends
							if (ioctlsocket(AcceptedHandle, FIONBIO, &NonBlock) == SOCKET_ERROR)

							{

								printf("ioctlsocket(FIONBIO) failed with error %d\n", WSAGetLastError());

								return 1;

							}

							else

								printf("ioctlsocket(FIONBIO) is OK!\n");

							
							

							if (SocketArray.CreateSocketInformation(AcceptedHandle,accepted_addrs) == FALSE)

							{

								printf("CreateSocketInformation(AcceptSocket) failed!\n");

								return 1;

							}

							else

								printf("Successfully connected to::\n");
							
							SocketArray.Endpoint[SocketArray.GetTotal() - 1].Print();




						}

						else

						{

							if (WSAGetLastError() != WSAEWOULDBLOCK)

							{

								printf("accept() failed with error %d\n", WSAGetLastError());

								return 1;

							}

							else

								printf("accept() is fine!\n");

						}

					}

					
				
					// Check each socket for Read and Write notification until the number

					// of sockets in Total is satisfied
					
					for (i = 0; i < SocketArray.GetTotal(); i++)

					{
						FD_ZERO(&WriteSet);

						FD_ZERO(&ReadSet);

						// Always look for connection attempts

						FD_SET(SocketArray.SocketArray[i].GetHandle(), &WriteSet);
						FD_SET(SocketArray.SocketArray[i].GetHandle(), &ReadSet);



						timeout.tv_sec = 0;
						timeout.tv_usec = 1000;

						
						
						// If the ReadSet is marked for this socket then this means data is available to be read on the socket
						 
						if ((select(SocketArray.SocketArray[i].GetHandle()+1, &ReadSet, NULL, NULL, &timeout)) == SOCKET_ERROR)	 //make handle readable

						{

							printf("select() returned with error %d\n", WSAGetLastError());

							return 1;

						} 

						if (FD_ISSET(SocketArray.SocketArray[i].GetHandle(), &ReadSet))

						{
						
	

							

							if (SocketArray.GetTotal() > 1)
							{
								if (SocketArray.SocketArray[i].SWSARecv() == PResult::P_Success)
								{

									std::cout << SocketArray.SocketArray[i].GetBufData() << std::endl;
									for (DWORD j = 0; j < SocketArray.GetTotal(); j++)
									{
										if (j != i)
										{
											FD_SET(SocketArray.SocketArray[j].GetHandle(), &WriteSet);
											SocketArray.SocketArray[j].SetBuffer(SocketArray.SocketArray[i].GetBufData());	  //COPY BUFFER DATA TO SOCKET

											if (j != i)
											{
												FD_SET(SocketArray.SocketArray[j].GetHandle(), &WriteSet);

												if (FD_ISSET(SocketArray.SocketArray[j].GetHandle(), &WriteSet))
												{
													if ((SocketArray.SocketArray[j].SWSASend()) != PResult::P_Success)		   //SEND BUFFER
													{
														printf("server could not forward data");
														break;
													}
													else
													{

														FD_ZERO(&ReadSet);

													}
												}

											}

										}
									}
									//RECEIVED BUFFER DATA IS SEND TO EVERY CONNECTED CLIENT SOCKET

								}
								else
								{
									std::cout << "socket handle::" << SocketArray.SocketArray[i].GetHandle() <<
										"connection was closed" << std::endl;
									SocketArray.FreeSocketInformation(i);

								}
							}
							else { Sleep(5000); }



						}
					}

					// If the WriteSet is marked on this socket then this means the internal

					// data buffers are available for more data
					

					
				}
			}
                      
            
            else
            {
                std::cout << "failed to bind or listen to socket" << std::endl;
            }
      
            ListeningSocket.close();
        }
        else
        {
            std::cerr << "failed to create socket" << std::endl;
        }
    }

    Network::Shutdown();
    return 0;
}
