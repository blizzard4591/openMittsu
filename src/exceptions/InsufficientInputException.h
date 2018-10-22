#ifndef OPENMITTSU_EXCEPTIONS_INSUFFICIENTINPUTEXCEPTION_H_
#define OPENMITTSU_EXCEPTIONS_INSUFFICIENTINPUTEXCEPTION_H_

#include "src/exceptions/BaseException.h"
#include "src/exceptions/ExceptionMacros.h"

OPENMITTSU_NEW_EXCEPTION(InsufficientInputExceptionImpl)
#define InsufficientInputException() InsufficientInputExceptionImpl(__FILE__, __LINE__)
 
#endif /* OPENMITTSU_EXCEPTIONS_INSUFFICIENTINPUTEXCEPTION_H_ */
