#ifndef OPENMITTSU_CRYPTO_FIXEDNONCES_H_
#define OPENMITTSU_CRYPTO_FIXEDNONCES_H_

#include "src/crypto/Nonce.h"

namespace openmittsu {
	namespace crypto {

		class FixedNonces {
		public:
			static Nonce getFixedGroupImageNonce();
		};

	}
}

#endif // OPENMITTSU_CRYPTO_FIXEDNONCES_H_
