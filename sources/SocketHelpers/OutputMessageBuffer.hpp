#pragma once
#include <cstdlib>
#include <string>

class OutputMessageBuffer
{
public:
    OutputMessageBuffer (size_t initialSize);
    virtual ~OutputMessageBuffer ();

    const char *GetConstCBuffer () const;
    char *GetCBuffer ();

    size_t GetCapacity () const;
    size_t GetSize () const;

    void WriteInt (int value);
    void WriteFloat (float value);
    void WriteString (const std::string &string);
    void WriteBuffer (const char *buffer, size_t size);

protected:

private:
    void UpdateCapacity (size_t neededAddition);

    char *cBuffer_;
    size_t capacity_;
    size_t size_;
};
