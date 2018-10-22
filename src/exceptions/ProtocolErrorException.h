#ifndef OPENMITTSU_EXCEPTIONS_PROTOCOLERROREXCEPTION_H_
#define OPENMITTSU_EXCEPTIONS_PROTOCOLERROREXCEPTION_H_

#include "src/exceptions/BaseException.h"
#include "src/exceptions/ExceptionMacros.h"

OPENMITTSU_NEW_EXCEPTION(ProtocolErrorExceptionImpl)
#define ProtocolErrorException() ProtocolErrorExceptionImpl(__FILE__, __LINE__)

#endif /* OPENMITTSU_EXCEPTIONS_PROTOCOLERROREXCEPTION_H_ */
