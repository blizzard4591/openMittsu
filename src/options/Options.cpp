#include "src/options/Options.h"

uint qHash(openmittsu::options::Options const& key, uint seed) {
	return ::qHash(static_cast<std::size_t>(key), seed);
}
