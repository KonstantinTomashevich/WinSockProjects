#pragma once
#include <cstdlib>
#include <string>

class InputMessageBuffer
{
public:
    explicit InputMessageBuffer (size_t maximumSize);
    virtual ~InputMessageBuffer ();

    size_t GetPosition () const;
    int IntFromPosition () const;
    float FloatFromPosition () const;
    std::string StringFromPosition () const;
    const char *GetConstCBuffer () const;

    char *GetCBuffer ();
    void SetPosition (size_t position);

    class Exceptions
    {
    public:
        class EndReached;
    };

protected:

private:
    char *cBuffer_;
    size_t position_;
    size_t maximumSize_;
};
