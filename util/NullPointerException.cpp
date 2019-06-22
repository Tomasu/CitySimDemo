#include "util/NullPointerException.h"


NullPointerException::~NullPointerException()
{

}

NullPointerException::NullPointerException(const std::string &message)
	: std::logic_error(message)
{

}

NullPointerException::NullPointerException(const char *message)
	: std::logic_error(message)
{

}

NullPointerException::NullPointerException(const NullPointerException &other )
	: std::logic_error(other)
{

}
