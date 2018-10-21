#ifndef OPENMITTSU_MESSAGES_CONTACT_CONTACTVIDEOIDANDIMAGEIDANDKEYMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_CONTACT_CONTACTVIDEOIDANDIMAGEIDANDKEYMESSAGECONTENT_H_

#include "src/messages/MessageContentFactory.h"
#include "src/messages/contact/ContactMessageContent.h"
#include "src/crypto/EncryptionKey.h"

#include <QByteArray>
#include <QtGlobal>

namespace openmittsu {
	namespace messages {
		namespace contact {

			class ContactVideoIdAndImageIdAndKeyMessageContent : public ContactMessageContent {
			public:
				ContactVideoIdAndImageIdAndKeyMessageContent(QByteArray const& videoBlobId, QByteArray const& imageBlobId, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 videoSizeInBytes, quint32 imageSizeInBytes);
				virtual ~ContactVideoIdAndImageIdAndKeyMessageContent();

				virtual ContactMessageContent* clone() const override;

				virtual bool hasPostReceiveCallbackTask() const override;

				virtual openmittsu::tasks::CallbackTask* getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const override;
				virtual MessageContent* integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const override;

				virtual QByteArray toPacketPayload() const override;
				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				QByteArray const& getVideoBlobId() const;
				QByteArray const& getImageBlobId() const;
				openmittsu::crypto::EncryptionKey const& getEncryptionKey() const;
				quint16 getLengthInSeconds() const;
				quint32 getVideoSizeInBytes() const;
				quint32 getImageSizeInBytes() const;

				friend class TypedMessageContentFactory<ContactVideoIdAndImageIdAndKeyMessageContent>;
			private:
				QByteArray const m_videoBlobId;
				QByteArray const m_imageBlobId;
				openmittsu::crypto::EncryptionKey const m_encryptionKey;
				quint16 const m_lengthInSeconds;
				quint32 const m_videoSizeInBytes;
				quint32 const m_imageSizeInBytes;

				static bool m_registrationResult;

				ContactVideoIdAndImageIdAndKeyMessageContent();
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_CONTACT_CONTACTVIDEOIDANDIMAGEIDANDKEYMESSAGECONTENT_H_
