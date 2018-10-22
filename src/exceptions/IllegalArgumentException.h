#ifndef OPENMITTSU_EXCEPTIONS_ILLEGALARGUMENTEXCEPTION_H_
#define OPENMITTSU_EXCEPTIONS_ILLEGALARGUMENTEXCEPTION_H_

#include "src/exceptions/BaseException.h"
#include "src/exceptions/ExceptionMacros.h"

OPENMITTSU_NEW_EXCEPTION(IllegalArgumentExceptionImpl)
#define IllegalArgumentException() IllegalArgumentExceptionImpl(__FILE__, __LINE__)

#endif /* OPENMITTSU_EXCEPTIONS_ILLEGALARGUMENTEXCEPTION_H_ */
