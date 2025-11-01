#include "src/options/OptionGroups.h"

namespace openmittsu {
	namespace options {
		utility::QtHashSizeType qHash(openmittsu::options::OptionGroups const& key, utility::QtHashSizeType seed) {
			return ::qHash(static_cast<std::size_t>(key), seed);
		}
	}
}
