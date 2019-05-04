#pragma once
#include <winsock2.h>
#include <string>

class ClientSocket;
class ServerSocket
{
public:
    explicit ServerSocket (const std::string &serverName);
    virtual ~ServerSocket ();

    void Disconnect ();
    ClientSocket *WaitForNextClient ();
    ClientSocket *WaitForNextClientNonBlocking ();

    class Exceptions
    {
    public:
        class UnableToParseAddress;
        class UnableToCreateCSocket;
        class UnableToBindCSocket;
        class UnableToListen;
        class UnableToAccept;
        class UnableToDisconnect;
    };

protected:

private:
    SOCKET cSocket_;
};
