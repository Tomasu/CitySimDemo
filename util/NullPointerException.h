#ifndef NULLPOINTEREXCEPTION_H
#define NULLPOINTEREXCEPTION_H

#include <stdexcept>

/**
 * @todo write docs
 */
class NullPointerException : public std::logic_error
{
	public:
		/**
		* Destructor
		*/
		~NullPointerException();

		/**
		* Constructor
		*
		* @param __arg TODO
		*/
		NullPointerException(const std::string &message);

		/**
		* Constructor
		*
		* @param  TODO
		*/
		NullPointerException(const char *message);

		/**
		* Constructor
		*
		* @param  TODO
		*/
		NullPointerException(const NullPointerException &other);


};

#endif // NULLPOINTEREXCEPTION_H
