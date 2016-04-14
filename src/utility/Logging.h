#ifndef OPENMITTSU_UTILITY_LOGGING_H_
#define OPENMITTSU_UTILITY_LOGGING_H_

#include "Config.h"

// Enable debug log level depending on the chosen configuration
#if ((defined(_MSC_VER) && defined(_DEBUG)) || (defined(OPENMITTSU_CONFIG_DEBUG_FLAG_SET)))
#	ifndef SPDLOG_DEBUG_ON
#		define SPDLOG_DEBUG_ON
#	endif
#	define OPENMITTSU_LOGGING_LEVEL spdlog::level::debug
#else
#	define OPENMITTSU_LOGGING_LEVEL spdlog::level::warn
#endif

#include <spdlog/spdlog.h>

#define OPENMITTSU_LOGGING_LOGGER_MAIN_NAME "main"
#define OPENMITTSU_LOGGING_MAX_FILESIZE (5 * 1024 * 1024) // 5 MBytes
#define OPENMITTSU_LOGGING_MAX_FILECOUNT (3) // 3 Files

#define LOGGER() spdlog::get(OPENMITTSU_LOGGING_LOGGER_MAIN_NAME)
#define LOGGER_DEBUG(...)  do { SPDLOG_DEBUG(LOGGER(), __VA_ARGS__) } while (false)

#endif // OPENMITTSU_UTILITY_LOGGING_H_