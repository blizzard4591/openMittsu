#pragma once

#include <cstddef>
#include <QtVersionChecks>

namespace openmittsu {
	namespace utility {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		using QtHashSizeType = std::size_t;
#else
		using QtHashSizeType = uint;
#endif
	}
}
