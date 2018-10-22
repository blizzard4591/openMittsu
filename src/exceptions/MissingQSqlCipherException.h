#ifndef OPENMITTSU_EXCEPTIONS_MISSINGQSQLCIPHEREXCEPTION_H_
#define OPENMITTSU_EXCEPTIONS_MISSINGQSQLCIPHEREXCEPTION_H_

#include "src/exceptions/BaseException.h"
#include "src/exceptions/ExceptionMacros.h"

OPENMITTSU_NEW_EXCEPTION(MissingQSqlCipherExceptionImpl)
#define MissingQSqlCipherException() MissingQSqlCipherExceptionImpl(__FILE__, __LINE__)

#endif /* OPENMITTSU_EXCEPTIONS_MISSINGQSQLCIPHEREXCEPTION_H_ */
