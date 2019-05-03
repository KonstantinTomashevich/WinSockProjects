#pragma once
#include <winsock2.h>
#include <string>

class ClientSocket
{
public:
    explicit ClientSocket (const std::string &serverNode, const std::string &serverName);
    explicit ClientSocket (SOCKET cSocket);
    virtual ~ClientSocket ();

    class Exceptions
    {
    public:
        class UnableToParseAddress;
        class UnableToCreateCSocket;
        class UnableToConnect;
        class UnableToDisconnect;
    };

protected:

private:
    SOCKET cSocket_;
};
