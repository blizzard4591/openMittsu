#include "src/exceptions/BaseException.h"

namespace openmittsu {
	namespace exceptions {

		BaseException::BaseException() : std::exception() {
			// Intentionally left empty.
		}

		BaseException::BaseException(BaseException const& other) : std::exception(other), stream(other.stream.str()) {
			// Intentionally left empty.
		}

		BaseException::BaseException(char const* cstr) {
			stream << cstr;
		}

		BaseException::~BaseException() {
			// Intentionally left empty.
		}

		const char* BaseException::what() const NOEXCEPT {
			std::string errorString = this->stream.str();
			char* result = new char[errorString.size() + 1];
			result[errorString.size()] = '\0';
			std::copy(errorString.begin(), errorString.end(), result);
			return result;
		}

		const char* BaseException::name() const NOEXCEPT {
			return "BaseException";
		}

	}
}
