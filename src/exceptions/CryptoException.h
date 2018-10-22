#ifndef OPENMITTSU_EXCEPTIONS_CRYPTOEXCEPTION_H_
#define OPENMITTSU_EXCEPTIONS_CRYPTOEXCEPTION_H_

#include "src/exceptions/BaseException.h"
#include "src/exceptions/ExceptionMacros.h"

OPENMITTSU_NEW_EXCEPTION(CryptoExceptionImpl)
#define CryptoException() CryptoExceptionImpl(__FILE__, __LINE__)

#endif /* OPENMITTSU_EXCEPTIONS_CRYPTOEXCEPTION_H_ */
