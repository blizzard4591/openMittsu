#include "src/options/OptionGroups.h"

uint qHash(openmittsu::options::OptionGroups const& key, uint seed) {
	return ::qHash(static_cast<std::size_t>(key), seed);
}
