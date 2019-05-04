#include "ServerSocket.hpp"
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
#include "ClientSocket.hpp"

typedef struct addrinfo addrinfo;

ServerSocket::ServerSocket (const std::string &serverName)
{
    int returnCode;
    addrinfo *resultAddress = NULL;
    addrinfo hints;

    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    returnCode = getaddrinfo (NULL, serverName.c_str (), &hints, &resultAddress);

    if (returnCode)
    {
        throw UniversalException <Exceptions::UnableToParseAddress> (std::string (__FILE__) + ":" +
            std::to_string (__LINE__) + "    Unable to parse address! Code: " +
            std::to_string (returnCode) + ".");
    }

    cSocket_ = socket (resultAddress->ai_family, resultAddress->ai_socktype, resultAddress->ai_protocol);
    if (cSocket_ == INVALID_SOCKET)
    {
        freeaddrinfo (resultAddress);
        throw UniversalException <Exceptions::UnableToCreateCSocket> (std::string (__FILE__) + ":" +
            std::to_string (__LINE__) + "    Unable create CSocket! Error: " +
            std::to_string (WSAGetLastError ()) + ".");
    }

    returnCode = bind (cSocket_, resultAddress->ai_addr, (int) resultAddress->ai_addrlen);
    freeaddrinfo (resultAddress);

    if (returnCode == SOCKET_ERROR)
    {
        throw UniversalException <Exceptions::UnableToBindCSocket> (std::string (__FILE__) + ":" +
            std::to_string (__LINE__) + "    Unable to bind CSocket! Error: " +
            std::to_string (WSAGetLastError ()) + ".");
    }
}

ServerSocket::~ServerSocket ()
{
    closesocket (cSocket_);
}

void ServerSocket::Disconnect ()
{
    int returnCode = shutdown (cSocket_, SD_BOTH);
    if (returnCode == SOCKET_ERROR)
    {
        throw UniversalException <Exceptions::UnableToDisconnect> (std::string (__FILE__) + ":" +
            std::to_string (__LINE__) + "    Unable disconnect socket! Code: " +
            std::to_string (returnCode) + ".");
    }
}

ClientSocket *ServerSocket::WaitForNextClient ()
{
    if (listen (cSocket_, SOMAXCONN) == SOCKET_ERROR)
    {
        throw UniversalException <Exceptions::UnableToListen> (std::string (__FILE__) + ":" +
            std::to_string (__LINE__) + "    Unable to listen to CSocket! Error: " +
            std::to_string (WSAGetLastError ()) + ".");
    }

    SOCKET clientSocket = accept (cSocket_, NULL, NULL);
    if (clientSocket == INVALID_SOCKET)
    {
        throw UniversalException <Exceptions::UnableToAccept> (std::string (__FILE__) + ":" +
            std::to_string (__LINE__) + "    Unable to accept new connection! Error: " +
            std::to_string (WSAGetLastError ()) + ".");
    }

    return new ClientSocket (clientSocket);
}
