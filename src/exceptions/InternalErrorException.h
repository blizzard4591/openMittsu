#ifndef OPENMITTSU_EXCEPTIONS_INTERNALERROREXCEPTION_H_
#define OPENMITTSU_EXCEPTIONS_INTERNALERROREXCEPTION_H_

#include "src/exceptions/BaseException.h"
#include "src/exceptions/ExceptionMacros.h"

OPENMITTSU_NEW_EXCEPTION(InternalErrorExceptionImpl)
#define InternalErrorException() InternalErrorExceptionImpl(__FILE__, __LINE__)
 
#endif /* OPENMITTSU_EXCEPTIONS_INTERNALERROREXCEPTION_H_ */
