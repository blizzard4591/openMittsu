#include "src/options/Options.h"

namespace openmittsu {
	namespace options {
		utility::QtHashSizeType qHash(openmittsu::options::Options const& key, utility::QtHashSizeType seed) {
			return ::qHash(static_cast<std::size_t>(key), seed);
		}
	}
}
