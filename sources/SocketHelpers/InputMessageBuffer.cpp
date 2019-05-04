#include "InputMessageBuffer.hpp"
#include <Utils/UniversalException.hpp>

InputMessageBuffer::InputMessageBuffer (size_t maximumSize)
    : position_ (0),
      maximumSize_ (maximumSize)
{
    cBuffer_ = new char[maximumSize];
}

InputMessageBuffer::~InputMessageBuffer ()
{
    delete[] cBuffer_;
}

size_t InputMessageBuffer::GetPosition () const
{
    return position_;
}

size_t InputMessageBuffer::GetMaximumSize () const
{
    return maximumSize_;
}

int InputMessageBuffer::IntFromPosition () const
{
    CheckPosition ();
    return *(int *) (cBuffer_ + position_);
}

float InputMessageBuffer::FloatFromPosition () const
{
    CheckPosition ();
    return *(float *) (cBuffer_ + position_);
}

std::string InputMessageBuffer::StringFromPosition () const
{
    CheckPosition ();
    return std::string (cBuffer_ + position_);
}

const char *InputMessageBuffer::GetConstCBuffer () const
{
    return cBuffer_;
}

char *InputMessageBuffer::GetCBuffer ()
{
    return cBuffer_;
}

void InputMessageBuffer::SetPosition (size_t position)
{
    position_ = position;
}

int InputMessageBuffer::NextInt ()
{
    int value = IntFromPosition ();
    SetPosition (GetPosition () + sizeof (int));
    return value;
}

float InputMessageBuffer::NextFloat ()
{
    float value = FloatFromPosition ();
    SetPosition (GetPosition () + sizeof (float));
    return value;
}

std::string InputMessageBuffer::NextString ()
{
    std::string value = StringFromPosition ();
    SetPosition (GetPosition () + value.size () + 1);
    return value;
}

void InputMessageBuffer::CopyNextBuffer (char *to, size_t size)
{
    CheckPosition ();
    const char *source = GetConstCBuffer () + position_;
    memcpy (to, source, size);
}

void InputMessageBuffer::CheckPosition () const
{
    if (position_ >= maximumSize_)
    {
        throw UniversalException <Exceptions::EndReached> (std::string (__FILE__) + ":" +
            std::to_string (__LINE__) + "    Unable to read from position " +
            std::to_string (position_) + " as it higher than max " + std::to_string (maximumSize_) + ".");
    }
}
