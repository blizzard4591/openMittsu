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
			errorString = this->stream.str();
			return errorString.c_str();
		}

		const char* BaseException::name() const NOEXCEPT {
			return "BaseException";
		}

	}
}
