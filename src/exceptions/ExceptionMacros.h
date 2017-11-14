#ifndef OPENMITTSU_EXCEPTIONS_EXCEPTIONMACROS_H_
#define OPENMITTSU_EXCEPTIONS_EXCEPTIONMACROS_H_

/*!
 * Macro to generate descendant exception classes. As all classes are nearly the same, this makes changing common
 * features much easier.
 */
#define OPENMITTSU_NEW_EXCEPTION(exception_name) namespace openmittsu { \
namespace exceptions { \
class exception_name : public BaseException { \
public: \
exception_name() : BaseException() { \
} \
exception_name(char const* cstr) : BaseException(cstr) { \
} \
exception_name(exception_name const& cp) : BaseException(cp) { \
} \
~exception_name() throw() { \
} \
template<typename T> \
exception_name& operator<<(T const& var) { \
	this->stream << var; \
	return *this; \
} \
virtual const char* name() const NOEXCEPT override { \
	return #exception_name; \
} \
}; \
} \
}

#endif /* OPENMITTSU_EXCEPTIONS_EXCEPTIONMACROS_H_ */
