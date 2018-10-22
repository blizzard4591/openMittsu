#ifndef OPENMITTSU_EXCEPTIONS_INVALIDPASSWORDORDATABASEEXCEPTION_H_
#define OPENMITTSU_EXCEPTIONS_INVALIDPASSWORDORDATABASEEXCEPTION_H_

#include "src/exceptions/BaseException.h"
#include "src/exceptions/ExceptionMacros.h"

OPENMITTSU_NEW_EXCEPTION(InvalidPasswordOrDatabaseExceptionImpl)
#define InvalidPasswordOrDatabaseException() InvalidPasswordOrDatabaseExceptionImpl(__FILE__, __LINE__)
 
#endif /* OPENMITTSU_EXCEPTIONS_INVALIDPASSWORDORDATABASEEXCEPTION_H_ */
