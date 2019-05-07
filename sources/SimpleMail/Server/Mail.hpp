#pragma once
#include <string>
#include <queue>

class Mail
{
public:
    typedef struct
    {
        std::string fromWhom;
        std::string theme;
        std::string text;
    } Message;

    Mail (const std::string &login, const std::string &password);
    virtual ~Mail () = default;

    const std::string &GetLogin () const;
    const std::string &GetPassword () const;
    int GetMessagesCount () const;

    const Message &TopMessage () const;
    void PopMessage ();
    void PushMessage (const Message &message);

protected:

private:
    std::string login_;
    std::string password_;
    std::queue <Message> messages_;
};
