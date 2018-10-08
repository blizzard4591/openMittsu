#pragma once

#define SINGLE_ARG(...) __VA_ARGS__
#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)
#define OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(funcName, ...) do { \
auto ptr = m_database.lock(); \
if (!ptr) { \
	LOGGER()->error("Tried aquiring database pointer in wrapper for {} and failed!", STRINGIFY(funcName)); \
	throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for " << STRINGIFY(funcName) << " and failed!"; \
} \
 \
if (!QMetaObject::invokeMethod(ptr.get(), STRINGIFY(funcName), __VA_ARGS__)) { \
	throw openmittsu::exceptions::InternalErrorException() << "Could not invoke " << STRINGIFY(funcName) << " from Database Wrapper"; \
}\
} while (false)

#define OPENMITTSU_DATABASEWRAPPER_WRAP_VOID_NOARGS(funcName) do { \
auto ptr = m_database.lock(); \
if (!ptr) { \
	LOGGER()->error("Tried aquiring database pointer in wrapper for {} and failed!", STRINGIFY(funcName)); \
	throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for " << STRINGIFY(funcName) << " and failed!"; \
} \
 \
if (!QMetaObject::invokeMethod(ptr.get(), STRINGIFY(funcName))) { \
	throw openmittsu::exceptions::InternalErrorException() << "Could not invoke " << STRINGIFY(funcName) << " from Database Wrapper"; \
}\
} while (false)

#define OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(funcName, returnType, ...) do { \
returnType returnVal; \
 \
auto ptr = m_database.lock(); \
if (!ptr) { \
	LOGGER()->error("Tried aquiring database pointer in wrapper for {} and failed!", STRINGIFY(funcName)); \
	throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for " << STRINGIFY(funcName) << " and failed!"; \
} \
 \
if (!QMetaObject::invokeMethod(ptr.get(), STRINGIFY(funcName), Qt::BlockingQueuedConnection, Q_RETURN_ARG(returnType, returnVal), __VA_ARGS__)) { \
	throw openmittsu::exceptions::InternalErrorException() << "Could not invoke " << STRINGIFY(funcName) << " from Database Wrapper"; \
} \
 \
return returnVal; \
} while (false)

#define OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN_NOARGS(funcName, returnType) do { \
returnType returnVal; \
 \
auto ptr = m_database.lock(); \
if (!ptr) { \
	LOGGER()->error("Tried aquiring database pointer in wrapper for {} and failed!", STRINGIFY(funcName)); \
	throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for " << STRINGIFY(funcName) << " and failed!"; \
} \
 \
if (!QMetaObject::invokeMethod(ptr.get(), STRINGIFY(funcName), Qt::BlockingQueuedConnection, Q_RETURN_ARG(returnType, returnVal))) { \
	throw openmittsu::exceptions::InternalErrorException() << "Could not invoke " << STRINGIFY(funcName) << " from Database Wrapper"; \
} \
 \
return returnVal; \
} while (false)

/*
	Wrapping for MessageCenter
*/
#define OPENMITTSU_MESSAGECENTERWRAPPER_WRAP_VOID(funcName, ...) do { \
auto ptr = m_messageCenter.lock(); \
if (!ptr) { \
	LOGGER()->error("Tried aquiring MessageCenter pointer in wrapper for {} and failed!", STRINGIFY(funcName)); \
	throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring MessageCenter pointer in wrapper for " << STRINGIFY(funcName) << " and failed!"; \
} \
 \
if (!QMetaObject::invokeMethod(ptr.get(), STRINGIFY(funcName), __VA_ARGS__)) { \
	throw openmittsu::exceptions::InternalErrorException() << "Could not invoke " << STRINGIFY(funcName) << " from MessageCenter Wrapper"; \
}\
} while (false)

#define OPENMITTSU_MESSAGECENTERWRAPPER_WRAP_VOID_NOARGS(funcName) do { \
auto ptr = m_messageCenter.lock(); \
if (!ptr) { \
	LOGGER()->error("Tried aquiring MessageCenter pointer in wrapper for {} and failed!", STRINGIFY(funcName)); \
	throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring MessageCenter pointer in wrapper for " << STRINGIFY(funcName) << " and failed!"; \
} \
 \
if (!QMetaObject::invokeMethod(ptr.get(), STRINGIFY(funcName))) { \
	throw openmittsu::exceptions::InternalErrorException() << "Could not invoke " << STRINGIFY(funcName) << " from MessageCenter Wrapper"; \
}\
} while (false)

#define OPENMITTSU_MESSAGECENTERWRAPPER_WRAP_RETURN(funcName, returnType, ...) do { \
returnType returnVal; \
 \
auto ptr = m_messageCenter.lock(); \
if (!ptr) { \
	LOGGER()->error("Tried aquiring MessageCenter pointer in wrapper for {} and failed!", STRINGIFY(funcName)); \
	throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring MessageCenter pointer in wrapper for " << STRINGIFY(funcName) << " and failed!"; \
} \
 \
if (!QMetaObject::invokeMethod(ptr.get(), STRINGIFY(funcName), Qt::BlockingQueuedConnection, Q_RETURN_ARG(returnType, returnVal), __VA_ARGS__)) { \
	throw openmittsu::exceptions::InternalErrorException() << "Could not invoke " << STRINGIFY(funcName) << " from MessageCenter Wrapper"; \
} \
 \
return returnVal; \
} while (false)

#define OPENMITTSU_MESSAGECENTERWRAPPER_WRAP_RETURN_NOARGS(funcName, returnType) do { \
returnType returnVal; \
 \
auto ptr = m_messageCenter.lock(); \
if (!ptr) { \
	LOGGER()->error("Tried aquiring MessageCenter pointer in wrapper for {} and failed!", STRINGIFY(funcName)); \
	throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring MessageCenter pointer in wrapper for " << STRINGIFY(funcName) << " and failed!"; \
} \
 \
if (!QMetaObject::invokeMethod(ptr.get(), STRINGIFY(funcName), Qt::BlockingQueuedConnection, Q_RETURN_ARG(returnType, returnVal))) { \
	throw openmittsu::exceptions::InternalErrorException() << "Could not invoke " << STRINGIFY(funcName) << " from MessageCenter Wrapper"; \
} \
 \
return returnVal; \
} while (false)
