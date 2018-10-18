#ifndef OPENMITTSU_MESSAGES_CONTACT_CONTACTAUDIOIDANDKEYMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_CONTACT_CONTACTAUDIOIDANDKEYMESSAGECONTENT_H_

#include "src/messages/MessageContentFactory.h"
#include "src/messages/contact/ContactMessageContent.h"
#include "src/crypto/EncryptionKey.h"

#include <QByteArray>
#include <QtGlobal>

namespace openmittsu {
	namespace messages {
		namespace contact {

			class ContactAudioIdAndKeyMessageContent : public ContactMessageContent {
			public:
				ContactAudioIdAndKeyMessageContent(QByteArray const& blobId, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 sizeInBytes);
				virtual ~ContactAudioIdAndKeyMessageContent();

				virtual ContactMessageContent* clone() const override;

				virtual bool hasPostReceiveCallbackTask() const override;

				virtual openmittsu::tasks::CallbackTask* getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const override;
				virtual MessageContent* integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const override;

				virtual QByteArray toPacketPayload() const override;
				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				QByteArray const& getBlobId() const;
				openmittsu::crypto::EncryptionKey const& getEncryptionKey() const;
				quint32 getBlobSizeInBytes() const;
				quint16 getLengthInSeconds() const;

				friend class TypedMessageContentFactory<ContactAudioIdAndKeyMessageContent>;
			private:
				QByteArray const m_blobId;
				openmittsu::crypto::EncryptionKey const m_encryptionKey;
				quint16 const m_lengthInSeconds;
				quint32 const m_sizeInBytes;

				static bool m_registrationResult;

				ContactAudioIdAndKeyMessageContent();
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_CONTACT_CONTACTAUDIOIDANDKEYMESSAGECONTENT_H_
