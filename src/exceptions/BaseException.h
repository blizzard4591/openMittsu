#ifndef OPENMITTSU_EXCEPTIONS_BASEEXCEPTION_H_
#define OPENMITTSU_EXCEPTIONS_BASEEXCEPTION_H_

#include <exception>
#include <sstream>

#include "src/utility/OsDetection.h"

namespace openmittsu {
	namespace exceptions {

		/*!
		* This class represents the base class of all exception classes.
		*/
		class BaseException : public std::exception {
		public:
			/*!
			* Creates a base exception without a message.
			*/
			BaseException(char const* file, int line);

			/*!
			* Creates a base expression from the given exception.
			*
			* @param other The expression from which to copy-construct.
			*/
			BaseException(BaseException const& other);

			/*!
			* Adds the given string to the message of this exception.
			*/
			BaseException(char const* file, int line, char const* cstr);

			/*!
			* Declare a destructor to counter the "looser throw specificator" error
			*/
			virtual ~BaseException();

			/*!
			* Retrieves the message associated with this exception.
			*
			* @return The message associated with this exception.
			*/
			virtual const char* what() const NOEXCEPT override;

			/*!
			* Retrieves the type (name) of this exception.
			*
			* @return The class name associated with this exception.
			*/
			virtual const char* name() const NOEXCEPT;

		protected:
			// This stream stores the message of this exception.
			std::stringstream stream;
		private:
			// storage for the string backing the C string returned by what()
			mutable std::string errorString;

			BaseException() = delete;
		};

	}
}

#endif // OPENMITTSU_EXCEPTIONS_BASEEXCEPTION_H_
