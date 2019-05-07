#include <SocketHelpers/Init.hpp>
#include <SocketHelpers/ClientSocket.hpp>
#include <SocketHelpers/InputMessageBuffer.hpp>
#include <SocketHelpers/OutputMessageBuffer.hpp>
#include <Utils/CustomTerminate.hpp>
#include <SimpleMail/Defs.hpp>

std::string ServerMessageCodeAsString (int code);
void ProcessInitialMessage (ClientSocket *socket);
void MakeAuthRequest (ClientSocket *socket);
void MakeUnreadRequest (ClientSocket *socket);
void MakePushRequest (ClientSocket *socket);
void MakePopRequest (ClientSocket *socket);

std::string ServerMessageCodeAsString (int code)
{
    switch (code)
    {
    case STC_AUTH_REQUIRED:

        return "Auth required for this action!";

    case STC_UNREAD_COUNT:

        return "Unread messages count sent as additional data with this message.";

    case STC_POP_MESSAGE :

        return "Popped sent as additional data with this message.";

    case STC_TARGET_NOT_FOUND:

        return "Message target not found!";

    case STC_NO_MESSAGES:

        return "Your mail is empty!";

    case STC_UNABLE_TO_POP:

        return "Popped message is too big, unable to send it!";

    case STC_SUCCESSFULLY_PUSHED:

        return "Successfully sent!";

    case STC_UNABLE_TO_PUSH:

        return "Unable to sent!";

    default:

        return "Unknown message!";
    }
}

void ProcessInitialMessage (ClientSocket *socket)
{
    InputMessageBuffer inMessage (MAX_MESSAGE_SIZE);
    socket->Receive (inMessage);
    std::cout << "Server: " << ServerMessageCodeAsString (inMessage.NextInt ()) << std::endl;
}

void MakeAuthRequest (ClientSocket *socket)
{
    std::string login;
    std::string password;

    std::cout << "Login: ";
    std::cin >> login;

    std::cout << "Password: ";
    std::cin >> password;

    OutputMessageBuffer outMessage (MAX_MESSAGE_SIZE);
    outMessage.WriteInt (CTS_AUTH);
    outMessage.WriteString (login);
    outMessage.WriteString (password);
    socket->Send (outMessage);

    InputMessageBuffer inMessage (MAX_MESSAGE_SIZE);
    socket->Receive (inMessage);

    int code = inMessage.NextInt ();
    std::cout << "Server: " << ServerMessageCodeAsString (code) << std::endl;

    if (code == STC_UNREAD_COUNT)
    {
        std::cout << "Additional data: " << inMessage.NextInt () << std::endl;
    }
}

void MakeUnreadRequest (ClientSocket *socket)
{
    OutputMessageBuffer outMessage (MAX_MESSAGE_SIZE);
    outMessage.WriteInt (CTS_REQUEST_UNREAD);
    socket->Send (outMessage);

    InputMessageBuffer inMessage (MAX_MESSAGE_SIZE);
    socket->Receive (inMessage);

    int code = inMessage.NextInt ();
    std::cout << "Server: " << ServerMessageCodeAsString (code) << std::endl;

    if (code == STC_UNREAD_COUNT)
    {
        std::cout << "Additional data: " << inMessage.NextInt () << std::endl;
    }
}

void MakePushRequest (ClientSocket *socket)
{
    std::string toWhom;
    std::string theme;
    std::string text;

    char symbol;
    std::cout << "To whom: ";
    std::cin >> toWhom;

    std::cin.get ();
    std::cout << "Theme: ";
    while ((symbol = std::cin.get ()) != '\n')
    { theme += symbol; }

    std::cout << "Text: ";
    while ((symbol = std::cin.get ()) != '\n')
    { text += symbol; }

    OutputMessageBuffer outMessage (MAX_MESSAGE_SIZE);
    outMessage.WriteInt (CTS_PUSH_MESSAGE);
    outMessage.WriteString (toWhom);
    outMessage.WriteString (theme);
    outMessage.WriteString (text);
    socket->Send (outMessage);

    InputMessageBuffer inMessage (MAX_MESSAGE_SIZE);
    socket->Receive (inMessage);

    int code = inMessage.NextInt ();
    std::cout << "Server: " << ServerMessageCodeAsString (code) << std::endl;
}

void MakePopRequest (ClientSocket *socket)
{
    OutputMessageBuffer outMessage (MAX_MESSAGE_SIZE);
    outMessage.WriteInt (CTS_REQUEST_POP);
    socket->Send (outMessage);

    InputMessageBuffer inMessage (MAX_MESSAGE_SIZE);
    socket->Receive (inMessage);

    int code = inMessage.NextInt ();
    std::cout << "Server: " << ServerMessageCodeAsString (code) << std::endl;

    if (code == STC_POP_MESSAGE)
    {
        std::cout << "From whom: " << inMessage.NextString () << std::endl;
        std::cout << "Theme: " << inMessage.NextString () << std::endl;
        std::cout << "Text: " << inMessage.NextString () << std::endl;
    }
}

int main ()
{
    std::set_terminate (CustomTerminate);
    Init::LoadWindowsSocketLibrary ();

    std::string hostAddress;
    std::cout << "Input host address: ";
    std::cin >> hostAddress;

    ClientSocket *socket = nullptr;
    try
    {
        socket = new ClientSocket (hostAddress, PORT_STRING);
        ProcessInitialMessage (socket);

        std::string command;
        while (command != "quit")
        {
            std::cout << "Input command: ";
            std::cin >> command;

            if (command == "auth")
            { MakeAuthRequest (socket); }
            else if (command == "unread")
            { MakeUnreadRequest (socket); }
            else if (command == "push")
            { MakePushRequest (socket); }
            else if (command == "pop")
            { MakePopRequest (socket); }
        }

        socket->Disconnect ();
        delete socket;
    }
    catch (AnyUniversalException &exception)
    {
        delete socket;
        std::cerr << exception.GetException () << std::endl;
    }

    Init::UnloadWindowsSocketLibrary ();
    return 0;
}
