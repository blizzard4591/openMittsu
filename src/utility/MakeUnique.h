#ifndef OPENMITTSU_UTILITY_MAKEUNIQUE_H_
#define OPENMITTSU_UTILITY_MAKEUNIQUE_H_

#include "Config.h"

#include <memory>
#include <utility>

#ifndef OPENMITTSU_CONFIG_HAVE_MAKE_UNIQUE

// From: http://stackoverflow.com/q/7038357/341970
namespace std {
	template<typename T, typename... Args>
	std::unique_ptr<T> make_unique(Args&&... args) {
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	}
}

#endif

#endif // OPENMITTSU_UTILITY_MAKEUNIQUE_H_
