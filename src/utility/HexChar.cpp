#include "src/utility/HexChar.h"

namespace openmittsu {
	namespace utility {

		HexChar::HexChar(unsigned char uc) : c(uc) {
			// Intentionally left empty.
		}

		HexChar::HexChar(char sc) {
			c = *reinterpret_cast<unsigned char const*>(&sc);
		}

		unsigned char HexChar::getChar() const {
			return c;
		}

		std::ostream& operator<<(std::ostream& o, HexChar const& h) {
			auto flags = o.flags();
			o << std::hex << +h.getChar();
			o.flags(flags);
			return o;
		}

	}
}
