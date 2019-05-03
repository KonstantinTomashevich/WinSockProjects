#include "ClientSocket.hpp"
#pragma comment(lib, "Ws2_32.lib")
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <Utils/UniversalException.hpp>

typedef struct addrinfo addrinfo;

ClientSocket::ClientSocket (const std::string &serverNode, const std::string &serverName)
{
    int returnCode;
    addrinfo *resultAddress = nullptr;
    addrinfo hints;

    ZeroMemory (&hints, sizeof (hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    returnCode = getaddrinfo (serverNode.c_str (), serverName.c_str (), &hints, &resultAddress);
    if (returnCode)
    {
        throw UniversalException <Exceptions::UnableToParseAddress> (std::string (__FILE__) + ":" +
            std::to_string (__LINE__) + "    Unable to parse address! Code: " +
            std::to_string (returnCode) + ".");
    }

    for (addrinfo *ptr = resultAddress; ptr != nullptr; ptr = ptr->ai_next)
    {
        cSocket_ = socket (ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (cSocket_ == INVALID_SOCKET)
        {
            freeaddrinfo (resultAddress);
            throw UniversalException <Exceptions::UnableToCreateCSocket> (std::string (__FILE__) + ":" +
                std::to_string (__LINE__) + "    Unable create CSocket! Error: " +
                std::to_string (WSAGetLastError ()) + ".");
        }

        returnCode = connect (cSocket_, ptr->ai_addr, (int) ptr->ai_addrlen);
        if (returnCode == SOCKET_ERROR)
        {
            closesocket (cSocket_);
            cSocket_ = INVALID_SOCKET;
            continue;
        }

        break;
    }

    freeaddrinfo (resultAddress);
    if (cSocket_ == INVALID_SOCKET)
    {
        throw UniversalException <Exceptions::UnableToConnect> (std::string (__FILE__) + ":" +
            std::to_string (__LINE__) + "    Unable to establish connection!");
    }
}

ClientSocket::ClientSocket (SOCKET cSocket)
    : cSocket_ (cSocket)
{

}

ClientSocket::~ClientSocket ()
{
    int returnCode = shutdown (cSocket_, SD_BOTH);
    if (returnCode == SOCKET_ERROR)
    {
        // TODO: What about noexcept?!
        throw UniversalException <Exceptions::UnableToDisconnect> (std::string (__FILE__) + ":" +
            std::to_string (__LINE__) + "    Unable disconnect socket! Code: " +
            std::to_string (returnCode) + ".");
    }

    closesocket (cSocket_);
}
