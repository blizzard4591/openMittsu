#ifndef OPENMITTSU_EXCEPTIONS_UUIDALREADYEXISTSEXCEPTION_H_
#define OPENMITTSU_EXCEPTIONS_UUIDALREADYEXISTSEXCEPTION_H_

#include "src/exceptions/BaseException.h"
#include "src/exceptions/ExceptionMacros.h"

OPENMITTSU_NEW_EXCEPTION(UuidAlreadyExistsExceptionImpl)
#define UuidAlreadyExistsException() UuidAlreadyExistsExceptionImpl(__FILE__, __LINE__)
 
#endif /* OPENMITTSU_EXCEPTIONS_UUIDALREADYEXISTSEXCEPTION_H_ */
