#ifndef OPENMITTSU_MESSAGES_MESSAGEWITHPAYLOAD_H_
#define OPENMITTSU_MESSAGES_MESSAGEWITHPAYLOAD_H_

#include "src/messages/FullMessageHeader.h"
#include "src/messages/ContentType.h"
#include "src/crypto/FullCryptoBox.h"

#include <memory>

namespace openmittsu {
	namespace messages {
		class MessageWithEncryptedPayload;

		class MessageWithPayload {
		public:
			MessageWithPayload(FullMessageHeader const& messageHeader, QByteArray const& payload);
			MessageWithPayload(MessageWithPayload const& other);
			virtual ~MessageWithPayload();

			FullMessageHeader const& getMessageHeader() const;
			QByteArray const& getPayload() const;

			virtual MessageWithEncryptedPayload encrypt(std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const;
		private:
			FullMessageHeader const messageHeader;
			QByteArray const payload;

			// Disable the default constructor
			MessageWithPayload();
		};

	}
}

#endif // OPENMITTSU_MESSAGES_MESSAGEWITHPAYLOAD_H_
