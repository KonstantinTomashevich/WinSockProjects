#include "InputMessageBuffer.hpp"
#include <Utils/UniversalException.hpp>

InputMessageBuffer::InputMessageBuffer (size_t maximumSize)
    : position_ (0),
      maximumSize_ (maximumSize)
{
    cBuffer_ = new char [maximumSize];
}

InputMessageBuffer::~InputMessageBuffer ()
{
    delete [] cBuffer_;
}

size_t InputMessageBuffer::GetPosition () const
{
    return position_;
}

int InputMessageBuffer::IntFromPosition () const
{
    return *(int *) (cBuffer_ + position_);
}

float InputMessageBuffer::FloatFromPosition () const
{
    return *(float *) (cBuffer_ + position_);
}

std::string InputMessageBuffer::StringFromPosition () const
{
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
    if (position < maximumSize_)
    {
        position_ = position;
    }
    else
    {
        throw UniversalException <Exceptions::EndReached> (std::string (__FILE__) + ":" +
            std::to_string (__LINE__) + "    Unable to set position to " +
            std::to_string (position) + " as it higher than max " + std::to_string (maximumSize_) + ".");
    }
}
