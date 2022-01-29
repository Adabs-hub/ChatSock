// Client code
//AUTHOR:: ADABOGO EMMANEUL
#pragma comment(lib, "Pnet.lib")
#pragma comment(lib, "pdcurses.lib")
#pragma comment(lib, "ws2_32.lib")

#include <Pnet/Pnet.h>
#include <Windows.h>
#include <iostream>
#include <stdlib.h>
#include <curses.h>
#include <thread>
#include <ctime>
#include <time.h>
#include <vector>						
#include <sstream>

constexpr auto PORT           = 9034;
constexpr auto BROADCAST_PORT = 9956;
constexpr auto LINE_LENGHT    = 40;
#define override


   using namespace Pnet;
   void getline_async(WINDOW& textArea, WINDOW& insertBox, std::string& inputStr, bool& flag);
   char* current_time();
   void setConsoleSize();
   std::vector <std::string> toVector(std::string str);
   void display_recv(WINDOW* W, std::string senderName, std::string data);
   void display(WINDOW* W, std::string inputStr, int endline);


int main()
{ 
	setConsoleSize();
	
    if (Network::Initialize())
    {
        std::cerr << "winsock api intialized succesfully" << std::endl;
		bool server_located = false;
		sockaddr_in available_server_addr = {};
		DWORD reconnectTimer = 1;

		while (1)
		{
			if (!server_located)
			{
				available_server_addr = {};
				Socket udp;

				if (udp.create(SOCK_DGRAM, false) == ERR)
				{		

					std::cout << "udp socket creation failed" << std::endl;
					Sleep(1000 * reconnectTimer);
					reconnectTimer *= 2;
					udp.close();
				}
				std::cout << "WAITING FOR SERVER TO CONNECT ..." << std::endl;
				if (udp.Listener(IPEndPoint("0.0.0.0", BROADCAST_PORT), &available_server_addr) == OK)
				{
					//if(available_server_addr==null)
					server_located = true;
					available_server_addr.sin_port = htons(PORT);
					udp.close();
				}
				else
				{
					std::cout << "udp socket listening failed" << std::endl;
					Sleep(1000 * reconnectTimer);
					reconnectTimer *= 2;
					udp.close();
				}
			}
			else
			{
				Socket socket;
				if (socket.create(SOCK_STREAM, false) == OK)
				{

					std::cout << "socket created successfully" << std::endl;
					if (socket.Connect(&available_server_addr) == OK)
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
						{
							std::cout << "ioctlsocket(FIONBIO) is ok on sock " << (int)socket.GetHandle() << std::endl;
						}

						//Packet packet;
						bool flag = false;
						bool flagthread = true;
						timeval time_interval;
						FD_SET ReadSet;
						FD_SET WriteSet;
						std::string inputStr;
						std::thread th1;
						std::string filename = "data.log";


						TextStream textfile;
						//textfile.LoadUsers(filename); //Load previous charts from file
						//std::cout << "...\n... \n";

						initscr();
						if (has_colors())
						{
							start_color();
							init_pair(2, COLOR_RED,		COLOR_BLACK);
							init_pair(3, COLOR_GREEN,	COLOR_BLACK);
							init_pair(4, COLOR_BLUE,	COLOR_BLACK);
							init_pair(5, COLOR_CYAN,	COLOR_BLACK);
							init_pair(6, COLOR_YELLOW,  COLOR_BLACK);
							init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
							init_pair(8, COLOR_WHITE, COLOR_BLACK);
							
						}
						//char ch;
						//std::string str = "";

						int x = getmaxx(stdscr);
						int y = getmaxy(stdscr);

						noecho();

						WINDOW* MAIN = newwin(y - 15, x, 0, 0);
						refresh;
						scrollok(MAIN, TRUE);

						WINDOW* insertBox = newwin(10, x - 20, y - 11, 19);
						WINDOW* textArea = newwin(8, x - 24, y - 10, 21);
						scrollok(textArea, TRUE);

  /*
#ifdef XCURSES
	Xinitscr(argc, argv);
#else
	initscr();
	
#endif
  */
						while (true)
						{
							
							FD_ZERO(&ReadSet); // Prepare the Read and Write socket sets for network I/O notification
							FD_ZERO(&WriteSet);	// Always look for connection attempts 

							FD_SET(socket.GetHandle(), &ReadSet);	 // Set Read and Write notification for each socket based on the current state the buffer.  If there is data remaining in the
							FD_SET(socket.GetHandle(), &WriteSet);	// buffer then set the Write set otherwise the Read set


							time_interval.tv_sec = 0;
							time_interval.tv_usec = 1000;


							if ((select(1, &ReadSet, &WriteSet, NULL, &time_interval) == SOCKET_ERROR))
							{
								printf("select() returned with error %d\n", WSAGetLastError());
								return 1;
							}
							// Check each socket for Read and Write events until the number of sockets in Total is satisfied
							// If the ReadSet is marked for this socket then this means data
							// is available to be read on the socket

							if (FD_ISSET(socket.GetHandle(), &ReadSet))
							{
								if (socket.SWSARecv() == OK)
								{							
									//printf("\x1b[2A ");
								
									display_recv(MAIN, socket.GetSenderName(), socket.GetSocketData());
									Beep(1600, 100);
								//	waddstr(MAIN, socket.GetBufData().c_str());
								//	wrefresh(MAIN);

									///std::cout << "\t\t\t\t\t(" << socket.GetSenderName() << ") ::"<<current_time() << std::endl; //display sender's name on chat
								//	textfile.WriteToText(filename, "~/`/~3#(" + socket.GetSenderName() + ")::"+current_time()+"\n\n"); //save sender's name in textfile

									//std::cout << "\t\t\t\t\t  " << socket.GetBufData() << "\n\n" << std::endl;
									//textfile.WriteToText(filename, "~/`/~3# " + socket.GetBufData() + "\n");

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
									th1 = std::thread(getline_async,std::ref(*textArea),std::ref(*insertBox), std::ref(inputStr), std::ref(flag));
									flagthread = false;
								}
								if (flag)
								{
									th1.join();
									flagthread = true;
									if (inputStr.size() > 0)
									{
										//textfile.WriteToText(filename, "(you)::" + (const char)*current_time() + (const char)"\n" + inputStr + "\n");

										if (socket.SWSASend(inputStr) != OK)
										{
											printf("client was unable to send text\n");
										}
										waddstr(MAIN, "\n\n");
										display(MAIN, inputStr, 10);

										flag = false;
									}
									else
										beep();
								}

							}

							FD_ZERO(&ReadSet);
							FD_ZERO(&WriteSet);
							Sleep(200);
						}
					}
					else
					{
						socket.close();
						server_located = false;
					}
				}
				else
				{
					int err = WSAGetLastError();
					std::cout << "failed to connect socket" << std::endl;
					socket.close();
				}

			}
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


 void getline_async(WINDOW& textArea, WINDOW& insertBox, std::string &inputStr, bool &flag)
{
	 std::string str;
	 char ch; 

	 refresh();
	 wrefresh(&insertBox);
	 wrefresh(&textArea);
	//	std::getline(si, str);
	//  socket.SetBuffer(str);
	 while (1)
	 {
		 wattron(&insertBox, COLOR_PAIR(5));
		 box(&insertBox, 176, 176);
		 wattroff(&insertBox, COLOR_PAIR(5));
		 wrefresh(&insertBox);
		 box(&textArea, ' ', ' ');
		 wrefresh(&textArea);

		 ch = getch();

		 if (ch == '\n')
		 {
			 wclear(&textArea);
			 inputStr=str;
			 str.clear();
			 flag = true;
			 break;
		 }
															 
		 if (ch == '\b' && str.size() > 0) {

			 str.pop_back();
			 wclear(&textArea);
			 wrefresh(&textArea);
			
		 }
		 else
		 {
			 if (ch == '\b')
				 continue;
			 str.push_back(ch);
			
		 }

		 wclear (&textArea);
		 wrefresh(&textArea);
		 display(&textArea, str, 3);
		
		 continue;

	 }
 }	

 char *current_time()
 {
	 time_t current_time;
	 current_time = time(&current_time);
	 return ctime(&current_time);
 }

 void setConsoleSize()
 {
	 HWND console = GetConsoleWindow();
	 RECT consoleRect;
	 GetWindowRect(console, &consoleRect);
	 MoveWindow(console, consoleRect.left, consoleRect.top, 683, 700, TRUE);
	 SetConsoleTitleA("Text_Me");
 }


 std::vector <std::string> toVector(std::string str) {
	 std::vector <std::string> v;
	 std::istringstream iss(str);
	 for (std::string s; iss >> s;)
		 v.push_back(s);
	 return v;

 }


 void display_recv(WINDOW* W, std::string senderName, std::string data) {
	 unsigned i = 0;
	 std::vector <std::string> v = toVector(data);
	 size_t size = v.size();
	 int cur_x, max_x, dummy;
	 waddstr(W, "\n\n");
	 waddstr(W, "\t\t\t\t\t");

	 wattron(W, A_UNDERLINE | COLOR_PAIR(3));
	 waddstr(W, senderName.c_str());
	 wattroff(W, A_STANDOUT | COLOR_PAIR(3));

	 waddch(W, 176);

	 wattron(W, A_UNDERLINE | COLOR_PAIR(7));
	 waddstr(W, current_time());
	 wattroff(W, A_UNDERLINE | COLOR_PAIR(7));

	 while (i < size)
	 {

		 getyx(W, dummy, cur_x);
		 getmaxyx(W, dummy, max_x);
		 size_t w = max_x - cur_x;
		 if (i == 0)
			 waddstr(W, "\t\t\t\t\t");

		 if (w > v.at(i).length() + 2)
		 {
			 wattron(W, COLOR_PAIR(5));
			 waddstr(W, v.at(i).c_str());
			 wattroff(W, COLOR_PAIR(5));
			 waddch(W, ' ');
			 wrefresh(W);
			 i++;
		 }
		 else
		 {
			 waddstr(W, "\n\t\t\t\t\t");
			 //waddstr(W, "\t\t\t");
		 }
	 }

 }

 void display(WINDOW* W, std::string inputStr, int endline)
 {
	 if (endline != 3)
	 {
	 waddstr(W, "  ");
	 wattron(W, A_UNDERLINE | COLOR_PAIR(6));
	 waddstr(W, "YOU ");
	 wattroff(W, A_UNDERLINE | COLOR_PAIR(6));

	 wattron(W, A_UNDERLINE | COLOR_PAIR(7));
	 waddch(W, 177);
	 waddstr(W, current_time());
	 wattroff(W, A_UNDERLINE | COLOR_PAIR(7));

	 }
	 else
	 {
		 waddch(W, '\n');
		 waddstr(W, "  ");
	 }

	 unsigned i = 0;
	 std::vector <std::string> v = toVector(inputStr);
	 size_t size = v.size();
	 int cur_x, max_x, dummy;
	 
	waddstr(W, "  ");
	 while (i < size)
	 {
		 getyx(W, dummy, cur_x);
		 getmaxyx(W, dummy, max_x);
		 size_t w = (max_x-endline)- cur_x;

		 if (w > v.at(i).length() + 2)
		 {
			 wattron(W, COLOR_PAIR(5));
			 waddstr(W, v.at(i).c_str());
			 wattroff(W, COLOR_PAIR(5));
			 waddch(W, ' ');
			 wrefresh(W);
			 i++;
		 }
		 else
		 {
			 waddch(W, '\n');
			 waddstr(W, "  ");
			 wrefresh(W);
			 //waddstr(W, "\t\t\t");
		 }
	 }

 }