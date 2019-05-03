#include "Init.hpp"
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

WSAData wsaData;

namespace Init
{
void LoadWindowsSocketLibrary ()
{
    int returnCode;
    if ((returnCode = WSAStartup (MAKEWORD(2, 2), &wsaData)))
    {
        throw UniversalException <Exceptions::UnableToLoadWSA> (std::string (__FILE__) + ":" +
            std::to_string (__LINE__) + "    Unable to start win sock library! Error: " +
            std::to_string (returnCode) + ".");
    }
}

void UnloadWindowsSocketLibrary ()
{
    WSACleanup ();
}
}
