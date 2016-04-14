#include "protocol/FixedNonces.h"

Nonce FixedNonces::getFixedGroupImageNonce() {
	QByteArray nonceBytes = QByteArray(Nonce::getNonceLength(), 0x00);
	nonceBytes[nonceBytes.size() - 1] = 1;

	return Nonce(nonceBytes);
}
