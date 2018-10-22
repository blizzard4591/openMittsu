#ifndef OPENMITTSU_EXCEPTIONS_NOTCONNECTEDEXCEPTION_H_
#define OPENMITTSU_EXCEPTIONS_NOTCONNECTEDEXCEPTION_H_

#include "src/exceptions/BaseException.h"
#include "src/exceptions/ExceptionMacros.h"

OPENMITTSU_NEW_EXCEPTION(NotConnectedExceptionImpl)
#define NotConnectedException() NotConnectedExceptionImpl(__FILE__, __LINE__)

#endif /* OPENMITTSU_EXCEPTIONS_NOTCONNECTEDEXCEPTION_H_ */
