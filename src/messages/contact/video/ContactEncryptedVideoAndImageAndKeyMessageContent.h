#ifndef OPENMITTSU_MESSAGES_CONTACT_CONTACTENCRYPTEDVIDEOANDIMAGEANDKEYMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_CONTACT_CONTACTENCRYPTEDVIDEOANDIMAGEANDKEYMESSAGECONTENT_H_

#include "src/messages/contact/ContactMessageContent.h"
#include "src/crypto/EncryptionKey.h"

#include <QByteArray>

namespace openmittsu {
	namespace messages {
		namespace contact {

			class ContactEncryptedVideoAndImageAndKeyMessageContent : public ContactMessageContent {
			public:
				ContactEncryptedVideoAndImageAndKeyMessageContent(QByteArray const& encryptedVideo, QByteArray const& imageData, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 videoSizeInBytes);
				virtual ~ContactEncryptedVideoAndImageAndKeyMessageContent();

				virtual ContactMessageContent* clone() const override;

				virtual bool hasPreSendCallbackTask() const override;
				virtual openmittsu::tasks::CallbackTask* getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const override;

				virtual bool hasPostReceiveCallbackTask() const override;
				virtual openmittsu::tasks::CallbackTask* getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const override;

				virtual MessageContent* integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const override;

				virtual QByteArray toPacketPayload() const override;
				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				QByteArray const& getEncryptedVideoData() const;
				QByteArray const& getImageData() const;
				openmittsu::crypto::EncryptionKey const& getEncryptionKey() const;
				quint16 getLengthInSeconds() const;
				quint32 getVideoSizeInBytes() const;
			private:
				QByteArray const m_encryptedVideoData;
				QByteArray const m_imageData;
				openmittsu::crypto::EncryptionKey const m_encryptionKey;
				quint16 const m_lengthInSeconds;
				quint32 const m_videoSizeInBytes;
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_CONTACT_CONTACTENCRYPTEDVIDEOANDIMAGEANDKEYMESSAGECONTENT_H_
