#include "IPEndPoint.h"

#include <assert.h>


namespace Pnet
{
    IPEndPoint::IPEndPoint()
    {
    }
    IPEndPoint::IPEndPoint(const char* ip, unsigned short port)
    {
        this->port = port;

        in_addr address; //ip structure
        int result = inet_pton(AF_INET, ip, &address);
        if (result == 1)
        { 
            if (address.S_un.S_addr != INADDR_NONE)
            {
                ip_string = ip;
                hostname = ip;

                ip_byte.resize(sizeof(ULONG));
                memcpy(&ip_byte[0], &address.S_un.S_addr, sizeof(ULONG));

            

                ipversion = PVersion::IPv4;
                return;
            }
        }
        
        //Attempt to resolve hostname to ipv4 address
           addrinfo hints = {}; //hints wil filter the results we get back for getaddrinfo
            hints.ai_family = AF_INET;
            addrinfo* hostinfo = nullptr;
            result = getaddrinfo(ip, NULL, &hints, &hostinfo);
            
            if (result == 0)
            {

                sockaddr_in* host_addr =reinterpret_cast<sockaddr_in*>( hostinfo->ai_addr);

                //host_addr->sin_addr.S_un.S_addr
                ip_string.resize(16);
                inet_ntop(AF_INET, &host_addr->sin_addr, &ip_string[0], 16);

                hostname = ip;

                ULONG ip_long = host_addr->sin_addr.S_un.S_addr;
                ip_byte.resize(sizeof(ULONG));
                memcpy(&ip_byte[0], &ip_long, sizeof(ULONG));

                ipversion = PVersion::IPv4;

                freeaddrinfo(hostinfo);
                return;
            }
            
    }

    IPEndPoint::IPEndPoint(sockaddr* addr)
    {
        assert(addr->sa_family == AF_INET);
        sockaddr_in *server_addr_v4 = reinterpret_cast<sockaddr_in*>(addr);
        ipversion = PVersion::IPv4;
        port = ntohs(server_addr_v4->sin_port);

        ip_byte.resize(sizeof(ULONG));
        memcpy(&ip_byte[0], &server_addr_v4->sin_addr, sizeof(ULONG));

        ip_string.resize(16);
        inet_ntop(AF_INET, &server_addr_v4->sin_addr, &ip_string[0], 16);
        hostname = ip_string;

    }
    

    

    Pnet::PVersion Pnet::IPEndPoint::GetIPVersion()
    {
        return ipversion;
    }

    std::string IPEndPoint::GetHostName()
    {
        return hostname;
    }

    std::string IPEndPoint::GetIPString()
    {
        return ip_string;
    }

    std::vector<uint8_t> IPEndPoint::GetIPByte()
    {
        return ip_byte;
    }

    unsigned short IPEndPoint::GetPort()
    {
        return port;
    }

   

    sockaddr_in IPEndPoint::GetAddrInfoIPv4()
    {
        assert(ipversion == PVersion::IPv4);

        sockaddr_in addr = {};

        addr.sin_family = AF_INET;
        memcpy(&addr.sin_addr,&ip_byte[0] , sizeof(ULONG));
        addr.sin_port =htons( port);


        return addr;
    }
    void IPEndPoint::Print()
    {
        switch (ipversion)
        {
        case PVersion::IPv4:
            std::cout << "IP4 address" << std::endl;
            break;
        case PVersion::IPv6:
            std::cout << "IPv6 adress" << std::endl;

            break;
        default:
            std::cout << "IP version unknown" << std::endl;
        }
            std::cout << "hostname :" << hostname << std::endl;
            std::cout << "ip_string:" << ip_string << std::endl;
            std::cout << "port     :" << port << std::endl;
            std::cout << "ip_byte  :" << std::endl;
            for (auto & digit : ip_byte)
            {
                std::cout << (int)digit << std::endl;
            }
            
        
    }
}
