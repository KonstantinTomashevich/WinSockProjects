#pragma once
#include <winsock2.h>
#include <string>
#include "InputMessageBuffer.hpp"
#include "OutputMessageBuffer.hpp"

class ClientSocket
{
public:
    ClientSocket (const std::string &serverNode, const std::string &serverName);
    explicit ClientSocket (SOCKET cSocket);
    virtual ~ClientSocket ();

    void Disconnect ();
    bool AnyDataReceived () const;
    int Receive (InputMessageBuffer &message);
    int Send (const OutputMessageBuffer &message);

    class Exceptions
    {
    public:
        class UnableToParseAddress;
        class UnableToCreateCSocket;
        class UnableToConnect;
        class UnableToDisconnect;
        class ErrorDuringReceive;
        class ErrorDuringSend;
    };

protected:

private:
    SOCKET cSocket_;
};
