#ifndef OPENMITTSU_MESSAGES_CONTACT_CONTACTENCRYPTEDIMAGEANDKEYANDIMAGEIDMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_CONTACT_CONTACTENCRYPTEDIMAGEANDKEYANDIMAGEIDMESSAGECONTENT_H_

#include "src/messages/contact/ContactMessageContent.h"
#include "src/crypto/Nonce.h"

#include <QByteArray>

namespace openmittsu {
	namespace messages {
		namespace contact {

			class ContactEncryptedImageAndKeyAndImageIdMessageContent : public ContactMessageContent {
			public:
				ContactEncryptedImageAndKeyAndImageIdMessageContent(QByteArray const& encryptedImage, openmittsu::crypto::Nonce const& imageNonce, quint32 imageSizeInBytes, QByteArray const& blobId);
				virtual ~ContactEncryptedImageAndKeyAndImageIdMessageContent();

				virtual ContactMessageContent* clone() const override;

				virtual bool hasPostReceiveCallbackTask() const override;
				virtual openmittsu::tasks::CallbackTask* getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const override;
				virtual MessageContent* integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const override;

				virtual QByteArray toPacketPayload() const override;
				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				QByteArray const& getEncryptedImageData() const;
				openmittsu::crypto::Nonce const& getNonce() const;
				quint32 getImageSizeInBytes() const;
				QByteArray const& getImageId() const;
			private:
				QByteArray const encryptedImageData;
				openmittsu::crypto::Nonce const nonce;
				quint32 const sizeInBytes;
				QByteArray const blobId;
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_CONTACT_CONTACTENCRYPTEDIMAGEANDKEYANDIMAGEIDMESSAGECONTENT_H_
