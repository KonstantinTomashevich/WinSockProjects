#include <SocketHelpers/Init.hpp>
#include <SocketHelpers/ServerSocket.hpp>
#include <SocketHelpers/ClientSocket.hpp>
#include <SocketHelpers/InputMessageBuffer.hpp>
#include <SocketHelpers/OutputMessageBuffer.hpp>

#include <unordered_map>
#include <Utils/CustomTerminate.hpp>
#include <SimpleMail/Defs.hpp>
#include "Mail.hpp"

bool _working = true;

int _clientThreadsActive = 0;

HANDLE _registryMutex;

HANDLE _cerrMutex;

std::unordered_map <std::string, Mail *> _mails;

DWORD WINAPI ClientThread (LPVOID param);
DWORD WINAPI AskShutdownThread (LPVOID param);

void PrintUnknownMessageToCerr (int code);
Mail *TryAuth (InputMessageBuffer &message);
void RequestAuth (ClientSocket *client);
void SendUnread (ClientSocket *client, Mail *mail);
void PopMessage (ClientSocket *client, Mail *mail);
void ProceedWithMessage (ClientSocket *client, InputMessageBuffer &inMessage, Mail *mail);

DWORD WINAPI ClientThread (LPVOID param)
{
    ++_clientThreadsActive;
    auto *client = static_cast<ClientSocket *> (param);

    {
        OutputMessageBuffer message (512);
        message.WriteInt (STC_AUTH_REQUIRED);
        client->Send (message);
    }

    Mail *mail = nullptr;
    while (_working)
    {
        if (client->AnyDataReceived ())
        {
            InputMessageBuffer inMessage (MAX_MESSAGE_SIZE);
            if (client->Receive (inMessage) == 0)
            {
                break;
            }

            int code = inMessage.NextInt ();
            switch (code)
            {
            case CTS_AUTH:

                mail = TryAuth (inMessage);
                if (mail)
                { SendUnread (client, mail); }
                else
                { RequestAuth (client); }
                break;

            case CTS_PUSH_MESSAGE:

                if (mail)
                { ProceedWithMessage (client, inMessage, mail); }
                else
                { RequestAuth (client); }
                break;

            case CTS_REQUEST_POP:

                if (mail)
                { PopMessage (client, mail); }
                else
                { RequestAuth (client); }
                break;

            case CTS_REQUEST_UNREAD:

                if (mail)
                { SendUnread (client, mail); }
                else
                { RequestAuth (client); }
                break;

            default:

                PrintUnknownMessageToCerr (code);
            }
        }
    }

    try
    {
        client->Disconnect ();
    }
    catch (UniversalException <ClientSocket::Exceptions::UnableToDisconnect> &exception)
    {
        /* No action, client already disconnected */
    }

    delete client;
    --_clientThreadsActive;
    return 0;
}

DWORD WINAPI AskShutdownThread (LPVOID param)
{
    std::cout << "Press q to shutdown server...";
    while (getc (stdin) != 'q');
    _working = false;
    return 0;
}

void PrintUnknownMessageToCerr (int code)
{
    WaitForSingleObject (_cerrMutex, INFINITE);
    std::cerr << "Unknown message " << code << " received from client thread " <<
              GetCurrentThreadId () << "." << std::endl;
    ReleaseMutex (_cerrMutex);
}

Mail *TryAuth (InputMessageBuffer &message)
{
    std::string login = message.NextString ();
    std::string password = message.NextString ();
    Mail *mail = nullptr;

    WaitForSingleObject (_registryMutex, INFINITE);
    auto iterator = _mails.find (login);

    if (iterator == _mails.end ())
    {
        mail = new Mail (login, password);
        _mails.insert (std::make_pair (login, mail));
    }
    else if (iterator->second->GetPassword () == password)
    {
        mail = iterator->second;
    }

    ReleaseMutex (_registryMutex);
    return mail;
}

void RequestAuth (ClientSocket *client)
{
    OutputMessageBuffer message (512);
    message.WriteInt (STC_AUTH_REQUIRED);
    client->Send (message);
}

void SendUnread (ClientSocket *client, Mail *mail)
{
    OutputMessageBuffer message (512);
    message.WriteInt (STC_UNREAD_COUNT);
    message.WriteInt (mail->GetMessagesCount ());
    client->Send (message);
}

void PopMessage (ClientSocket *client, Mail *mail)
{
    OutputMessageBuffer message (512);
    if (mail->GetMessagesCount () > 0)
    {
        const Mail::Message &mailMessage = mail->TopMessage ();

        if (mailMessage.fromWhom.size () + mailMessage.text.size () +
            mailMessage.theme.size () + 3 + sizeof (int) < MAX_MESSAGE_SIZE)
        {
            message.WriteInt (STC_POP_MESSAGE);
            message.WriteString (mailMessage.fromWhom);
            message.WriteString (mailMessage.theme);
            message.WriteString (mailMessage.text);
        }
        else
        {
            message.WriteInt (STC_UNABLE_TO_POP);
        }

        mail->PopMessage ();
    }
    else
    {
        message.WriteInt (STC_NO_MESSAGES);
    }

    client->Send (message);
}

void ProceedWithMessage (ClientSocket *client, InputMessageBuffer &inMessage, Mail *mail)
{
    OutputMessageBuffer message (512);
    std::string toWhom = inMessage.NextString ();
    Mail::Message mailMessage;

    mailMessage.fromWhom = mail->GetLogin ();
    mailMessage.theme = inMessage.NextString ();
    mailMessage.text = inMessage.NextString ();

    if (mailMessage.fromWhom.size () + mailMessage.theme.size () +
        mailMessage.text.size () + 3 + sizeof (int) < MAX_MESSAGE_SIZE)
    {
        Mail *target = nullptr;
        WaitForSingleObject (_registryMutex, INFINITE);
        auto iterator = _mails.find (toWhom);

        if (iterator != _mails.end ())
        {
            target = iterator->second;
        }

        ReleaseMutex (_registryMutex);
        if (target != nullptr)
        {
            target->PushMessage (mailMessage);
            message.WriteInt (STC_SUCCESSFULLY_PUSHED);
        }
        else
        {
            message.WriteInt (STC_TARGET_NOT_FOUND);
        }
    }
    else
    {
        message.WriteInt (STC_UNABLE_TO_PUSH);
    }

    client->Send (message);
}

int main ()
{
    _registryMutex = CreateMutex (NULL, FALSE, NULL);
    _cerrMutex = CreateMutex (NULL, FALSE, NULL);

    std::set_terminate (CustomTerminate);
    Init::LoadWindowsSocketLibrary ();

    ServerSocket *server = new ServerSocket (PORT_STRING);
    CreateThread (NULL, 0, AskShutdownThread, NULL, 0, NULL);

    while (_working)
    {
        ClientSocket *client = server->WaitForNextClientNonBlocking ();
        if (client)
        {
            CreateThread (NULL, 0, ClientThread, (LPVOID) client, 0, NULL);
        }
    }

    while (_clientThreadsActive);
    delete server;
    Init::UnloadWindowsSocketLibrary ();

    for (auto &loginMailPair : _mails)
    {
        delete loginMailPair.second;
    }

    CloseHandle (_registryMutex);
    CloseHandle (_cerrMutex);
    return 0;
}
