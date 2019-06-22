#ifndef CITYSIM_PRECONDITIONS_H_GUARD
#define CITYSIM_PRECONDITIONS_H_GUARD

#include <stdexcept>

#include "NullPointerException.h"

template<typename T>
T *checkNotNull(T *arg, const std::string &message)
{
	if (arg != nullptr)
	{
		return arg;
	}

	throw NullPointerException(message);
	return nullptr;
}

template<typename T>
bool checkArgument(bool condition, const std::string &message)
{
	if (condition == true)
	{
		return true;
	}

	throw std::invalid_argument(message);
	return false;
}

#endif /* CITYSIM_PRECONDITIONS_H_GUARD */
