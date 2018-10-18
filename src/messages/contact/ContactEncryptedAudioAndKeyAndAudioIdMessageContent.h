#ifndef OPENMITTSU_MESSAGES_CONTACT_CONTACTENCRYPTEDAUDIOANDKEYANDAUDIOIDMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_CONTACT_CONTACTENCRYPTEDAUDIOANDKEYANDAUDIOIDMESSAGECONTENT_H_

#include "src/messages/contact/ContactMessageContent.h"
#include "src/crypto/EncryptionKey.h"

#include <QByteArray>

namespace openmittsu {
	namespace messages {
		namespace contact {

			class ContactEncryptedAudioAndKeyAndAudioIdMessageContent : public ContactMessageContent {
			public:
				ContactEncryptedAudioAndKeyAndAudioIdMessageContent(QByteArray const& encryptedBlob, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 imageSizeInBytes, QByteArray const& blobId);
				virtual ~ContactEncryptedAudioAndKeyAndAudioIdMessageContent();

				virtual ContactMessageContent* clone() const override;

				virtual bool hasPostReceiveCallbackTask() const override;
				virtual openmittsu::tasks::CallbackTask* getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const override;
				virtual MessageContent* integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const override;

				virtual QByteArray toPacketPayload() const override;
				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				QByteArray const& getEncryptedData() const;
				openmittsu::crypto::EncryptionKey const& getEncryptionKey() const;
				quint16 getLengthInSeconds() const;
				quint32 getSizeInBytes() const;
				QByteArray const& getBlobId() const;
			private:
				QByteArray const m_encryptedData;
				openmittsu::crypto::EncryptionKey const m_encryptionKey;
				quint16 const m_lengthInSeconds;
				quint32 const m_sizeInBytes;
				QByteArray const m_blobId;
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_CONTACT_CONTACTENCRYPTEDAUDIOANDKEYANDAUDIOIDMESSAGECONTENT_H_
