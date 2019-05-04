#include <SocketHelpers/Init.hpp>
#include <SocketHelpers/ServerSocket.hpp>
#include <SocketHelpers/ClientSocket.hpp>
#include <SocketHelpers/InputMessageBuffer.hpp>
#include <SocketHelpers/OutputMessageBuffer.hpp>
#include <Utils/CustomTerminate.hpp>
#include "Defs.hpp"

DWORD WINAPI ClientThread (LPVOID param)
{
    auto *client = static_cast<ClientSocket *> (param);
    {
        OutputMessageBuffer message (10);
        message.WriteString ("HelloWorld from server!");
        message.WriteInt (123);
        client->Send (message);
        Sleep (1000);
    }

    client->Disconnect ();
    delete client;
    return 0;
}

int main ()
{
    std::set_terminate (CustomTerminate);
    Init::LoadWindowsSocketLibrary ();

    ServerSocket *server = new ServerSocket (SERVER_NAME);
    int maxConnections = 1000;
    while (maxConnections--)
    {
        ClientSocket *client = server->WaitForNextClient ();
        CreateThread (NULL, 0, ClientThread, (LPVOID) client, 0, NULL);
    }

    server->Disconnect ();
    delete server;
    Init::UnloadWindowsSocketLibrary ();
    return 0;
}
