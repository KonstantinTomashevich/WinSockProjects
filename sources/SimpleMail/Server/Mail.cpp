#include "Mail.hpp"

Mail::Mail (const std::string &login, const std::string &password)
    : login_ (login),
      password_ (password),
      messages_ ()
{

}

const std::string &Mail::GetLogin () const
{
    return login_;
}

const std::string &Mail::GetPassword () const
{
    return password_;
}

int Mail::GetMessagesCount () const
{
    return messages_.size ();
}

const Mail::Message &Mail::TopMessage () const
{
    return messages_.front ();
}

void Mail::PopMessage ()
{
    messages_.pop ();
}

void Mail::PushMessage (const Mail::Message &message)
{
    messages_.push (message);
}
