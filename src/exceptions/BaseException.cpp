#include "src/exceptions/BaseException.h"

namespace openmittsu {
	namespace exceptions {

		BaseException::BaseException(char const* file, int line) : std::exception() {
			stream << file << ":" << line << ": ";
		}

		BaseException::BaseException(BaseException const& other) : std::exception(other), stream(other.stream.str()) {
			// Intentionally left empty.
		}

		BaseException::BaseException(char const* file, int line, char const* cstr) {
			stream << file << ":" << line << ": ";
			stream << cstr;
		}

		BaseException::~BaseException() {
			// Intentionally left empty.
		}

		const char* BaseException::what() const NOEXCEPT {
			errorString = this->stream.str();
			return errorString.c_str();
		}

		const char* BaseException::name() const NOEXCEPT {
			return "BaseException";
		}

	}
}
