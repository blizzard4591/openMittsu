#ifndef OPENMITTSU_EXCEPTIONS_INVALIDINPUTEXCEPTION_H_
#define OPENMITTSU_EXCEPTIONS_INVALIDINPUTEXCEPTION_H_

#include "src/exceptions/BaseException.h"
#include "src/exceptions/ExceptionMacros.h"

OPENMITTSU_NEW_EXCEPTION(InvalidInputExceptionImpl)
#define InvalidInputException() InvalidInputExceptionImpl(__FILE__, __LINE__)
 
#endif /* OPENMITTSU_EXCEPTIONS_INVALIDINPUTEXCEPTION_H_ */
