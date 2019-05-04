#pragma once
#include <cstdlib>
#include <string>

class InputMessageBuffer
{
public:
    explicit InputMessageBuffer (size_t maximumSize);
    virtual ~InputMessageBuffer ();

    size_t GetPosition () const;
    size_t GetMaximumSize () const;
    int IntFromPosition () const;
    float FloatFromPosition () const;
    std::string StringFromPosition () const;
    const char *GetConstCBuffer () const;

    char *GetCBuffer ();
    void SetPosition (size_t position);
    int NextInt ();
    float NextFloat ();
    std::string NextString ();
    void CopyNextBuffer (char *to, size_t size);

    class Exceptions
    {
    public:
        class EndReached;
    };

protected:

private:
    void CheckPosition () const;

    char *cBuffer_;
    size_t position_;
    size_t maximumSize_;
};
