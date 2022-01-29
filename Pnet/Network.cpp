#include "Network.h"
#include <iostream>
namespace Pnet
{

    bool Network::Initialize()
    {
        WSADATA wsadata;
        int result = WSAStartup(MAKEWORD (2, 2), &wsadata);
        if (result != 0)
        {
            int erro = WSAGetLastError();
            std::cerr << "failed to create winsock api" << std::endl;
            return PResult::OK;

        }
        return PResult::ERR;
    }
    bool Network::Shutdown()
    {
       int result= WSACleanup();
       if (result != 0)
       {
           int erro = WSAGetLastError();
           std::cerr << " windows api failed to shutdown" << std::endl;
           return PResult::ERR;
       }
       //std::cout << "windows api successfully shutdown" << std::endl;
        return PResult::OK;
    }
    
}