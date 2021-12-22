// Client code

#include <Pnet/Pnet.h>
#include <iostream>
#include <cstdlib>
#include <thread>
 #include <time.h>
#include <vector>																	
#include <Pnet/TextStream.h>

   using namespace Pnet;
   void getline_async( Socket &socket, std::istream &si,bool &flag);


int main()
{
    if (Network::Initialize())
    {
        std::cerr << "winsock api intialized succesfully" << std::endl;

        Socket socket;
        if (socket.create() == PResult::P_Success)
        {
			std::string ip = "127.0.0.1";
		
            std::cout << "socket created successfully" << std::endl;
			if (socket.Connect(IPEndPoint(ip.c_str(), 9034)) == PResult::P_Success)
			{
				std::cout << "socket connected successfully" << std::endl;

				DWORD NonBlock = 1;
				//Unbloking socket handle
				if (ioctlsocket(socket.GetHandle(), FIONBIO, &NonBlock) == SOCKET_ERROR)
				{

					printf("ioctlsocket(FIONBIO) failed with error %d\n", WSAGetLastError());
					socket.close();
					Network::Shutdown();
					return 0;

				}
				else
				{	std::cout << "ioctlsocket(FIONBIO) is ok on sock " << (int)socket.GetHandle()<<std::endl;}

				//Packet packet;
				bool flag = false;
				bool flagthread = true;
				 
				FD_SET ReadSet;
				FD_SET WriteSet;
				
				timeval time;

				std::string buf;
				std::thread th1;
				std::istream &si = std::cin;
				std::string filename="data";


				TextStream textfile;
				textfile.LoadUsers(filename); //Load previous charts from file

				
				while (true)
				{
					FD_ZERO(&ReadSet); // Prepare the Read and Write socket sets for network I/O notification
					FD_ZERO(&WriteSet);	// Always look for connection attempts 

					FD_SET(socket.GetHandle(), &ReadSet);	 // Set Read and Write notification for each socket based on the current state the buffer.  If there is data remaining in the
					FD_SET(socket.GetHandle(), &WriteSet);	// buffer then set the Write set otherwise the Read set

				
					time.tv_sec = 0;
					time.tv_usec = 1000;

					
					if ((select(1, &ReadSet, &WriteSet, NULL, &time) == SOCKET_ERROR))
					{
						printf("select() returned with error %d\n", WSAGetLastError());
						return 1;
					}
						// Check each socket for Read and Write events until the number of sockets in Total is satisfied
						// If the ReadSet is marked for this socket then this means data
						// is available to be read on the socket

					if (FD_ISSET(socket.GetHandle(), &ReadSet))
					{
						if (socket.SWSARecv() == PResult::P_Success)
						{		
							std::cout <<"\t\t\t\t\t("<< socket.GetSenderName()<<")"<< std::endl; //display sender's name on chat
							textfile.WriteToText(filename,"~/`/~3#("+ socket.GetSenderName()+")\n"); //save sender's name in textfile

							std::cout <<"\t\t\t\t\t  "<< socket.GetBufData() <<"\n\n"<< std::endl;
							textfile.WriteToText(filename,"~/`/~3# " + socket.GetBufData()+"\n");

						}
						else
							break; 

					}

					// If the WriteSet is marked on this socket then this means the internal
					// data buffers are available for more data
					if (FD_ISSET(socket.GetHandle(), &WriteSet))
					{
						
						if (flagthread)
						{
							th1 = std:: thread(getline_async, std::ref(socket), std::ref(si),std:: ref(flag));
							flagthread = false;		  
						}	
						if(flag)
						{		 
							  th1.join();
							  flagthread = true;
								if (socket.GetSockBufLen() > 0)
								{
									textfile.WriteToText(filename,socket.GetBufData()+"\n");

									if (socket.SWSASend() != PResult::P_Success)
									{
										printf("client was unable to send text\n");
										socket.Clearbuffer();
									}
									flag = false;
									socket.Clearbuffer();
								}
						}
							
					}


					FD_ZERO(&ReadSet);
					FD_ZERO(&WriteSet);

					Sleep(200);
				}
			}
			else
                 socket.close();
        }
        else
         {
            int err = WSAGetLastError();
            std::cout << "failed to connect socket" << std::endl;
         }
            
    }
     else
      {
          std::cerr << "failed to create socket" << std::endl;
      }
    

    Network::Shutdown();
    system("pause");
    return 0;
}
 void getline_async(Socket &socket, std::istream &si,bool &flag)
{
	 std::string str;
	
		std::getline(si, str);
		socket.SetBuffer(str);
		flag = true;
}
