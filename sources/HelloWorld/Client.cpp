#include <SocketHelpers/Init.hpp>
#include <SocketHelpers/ServerSocket.hpp>
#include <SocketHelpers/ClientSocket.hpp>
#include <SocketHelpers/InputMessageBuffer.hpp>
#include <SocketHelpers/OutputMessageBuffer.hpp>
#include <Utils/CustomTerminate.hpp>
#include "Defs.hpp"

int main ()
{
    std::set_terminate (CustomTerminate);
    Init::LoadWindowsSocketLibrary ();

    ClientSocket *socket = new ClientSocket ("localhost", SERVER_NAME);
    Sleep (2000);

    {
        InputMessageBuffer message (512);
        socket->Receive (message);
        std::string string = message.NextString ();
        int number = message.NextInt ();
        printf ("%s %d", string.c_str (), number);
    }

    socket->Disconnect ();
    delete socket;
    Init::UnloadWindowsSocketLibrary ();
    return 0;
}
