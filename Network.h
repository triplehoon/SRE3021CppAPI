// Network.h header file for C++ UDP
#pragma once

//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <system_error>
#include <string>
#include <iostream>

#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32")

class WSASession
{
public:
    WSASession()
    {
        int ret = WSAStartup(MAKEWORD(2, 2), &data);
        if (ret != 0)
            printf("WSAStartup Failed");
    }
    ~WSASession()
    {
        WSACleanup();
    }

private:
    WSAData data;
};

class UDPSocket
{
public:
    UDPSocket()
    {
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == INVALID_SOCKET)
            printf("Error opening socket");
    }
    ~UDPSocket()
    {
        closesocket(sock);
    }

    void SendTo(const std::string& address, unsigned short port, const char* buffer, int len, int flags = 0)
    {
        sockaddr_in add;
        add.sin_family = AF_INET;
        add.sin_addr.s_addr = inet_addr(address.c_str());
        //add.sin_addr.s_addr = inet_pton(AF_INET, "127.0.0.1", address.c_str());
        add.sin_port = htons(port);
        int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&add), sizeof(add));
        if (ret < 0)
            printf("sendto failed\n");
    }
    void SendTo(sockaddr_in& address, const char* buffer, int len, int flags = 0)
    {
        int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&address), sizeof(address));
        if (ret < 0)
            printf("sendto failed");
    }
    sockaddr_in RecvFrom(char* buffer, int len, int flags, int* outReadDataSize)
    {
        sockaddr_in from;
        int size = sizeof(from);
        int ret = recvfrom(sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&from), &size);
        
        // make the buffer zero terminated        
        *outReadDataSize = ret;
        return from;
    }
    void Bind(unsigned short port)
    {
        sockaddr_in add;
        add.sin_family = AF_INET;
        add.sin_addr.s_addr = htonl(INADDR_ANY);
        add.sin_port = htons(port);
        char option = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        struct timeval optVal = { 5, 0 };

        int optLen = sizeof(optVal);
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&optVal), optLen);


        int ret = bind(sock, reinterpret_cast<SOCKADDR*>(&add), sizeof(add));
        if (ret < 0)
            printf("Bind failed");
    }

private:
    SOCKET sock;
};