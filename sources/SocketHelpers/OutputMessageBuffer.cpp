#include <math.h>
#include "OutputMessageBuffer.hpp"

OutputMessageBuffer::OutputMessageBuffer (size_t initialSize)
    : capacity_ (initialSize),
      size_ (0)
{
    cBuffer_ = (char *) malloc (capacity_);
}

OutputMessageBuffer::~OutputMessageBuffer ()
{
    free (cBuffer_);
}

const char *OutputMessageBuffer::GetConstCBuffer () const
{
    return cBuffer_;
}

char *OutputMessageBuffer::GetCBuffer ()
{
    return cBuffer_;
}

size_t OutputMessageBuffer::GetCapacity () const
{
    return capacity_;
}

size_t OutputMessageBuffer::GetSize () const
{
    return size_;
}

void OutputMessageBuffer::WriteInt (int value)
{
    WriteBuffer ((char *) &value, sizeof (int));
}

void OutputMessageBuffer::WriteFloat (float value)
{
    WriteBuffer ((char *) &value, sizeof (float));
}

void OutputMessageBuffer::WriteString (const std::string &string)
{
    WriteBuffer (string.c_str (), string.size () + 1);
}

void OutputMessageBuffer::WriteBuffer (const char *buffer, size_t size)
{
    UpdateCapacity (size);
    memcpy (cBuffer_ + size_, buffer, size);
    size_ += size;
}

void OutputMessageBuffer::UpdateCapacity (size_t neededAddition)
{
    if (size_ + neededAddition > capacity_)
    {
        capacity_ += neededAddition < capacity_ / 5 ? capacity_ / 5 : neededAddition;
        cBuffer_ = (char *) realloc (cBuffer_, capacity_);
    }
}
