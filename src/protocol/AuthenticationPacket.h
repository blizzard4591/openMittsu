#ifndef OPENMITTSU_PROTOCOL_AUTHENTICATIONPACKET_H_
#define OPENMITTSU_PROTOCOL_AUTHENTICATIONPACKET_H_

#include "src/protocol/ContactId.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"

#include <memory>

namespace openmittsu {
	namespace protocol {

		class AuthenticationPacket {
		public:
			explicit AuthenticationPacket(ContactId const& sender, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox);
			virtual ~AuthenticationPacket();

			virtual QByteArray toPacket() const;
		private:
			ContactId const m_sender;
			std::shared_ptr<openmittsu::crypto::FullCryptoBox> const m_cryptoBox;
		};

	}
}

#endif // OPENMITTSU_PROTOCOL_AUTHENTICATIONPACKET_H_
