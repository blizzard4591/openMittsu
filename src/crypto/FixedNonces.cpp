#include "src/crypto/FixedNonces.h"

#include <QByteArray>

namespace openmittsu {
	namespace crypto {

		Nonce FixedNonces::getFixedNonce(int index) {
			QByteArray nonceBytes = QByteArray(Nonce::getNonceLength(), 0x00);
			nonceBytes[nonceBytes.size() - 1] = static_cast<char>(index);

			return Nonce(nonceBytes);
		}

	}
}
