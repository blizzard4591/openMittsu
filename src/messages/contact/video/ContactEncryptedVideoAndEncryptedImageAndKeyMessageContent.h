#ifndef OPENMITTSU_MESSAGES_CONTACT_CONTACTENCRYPTEDVIDEOANDENCRYPTEDIMAGEANDKEYMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_CONTACT_CONTACTENCRYPTEDVIDEOANDENCRYPTEDIMAGEANDKEYMESSAGECONTENT_H_

#include "src/messages/contact/ContactMessageContent.h"
#include "src/crypto/EncryptionKey.h"

#include <QByteArray>

namespace openmittsu {
	namespace messages {
		namespace contact {

			class ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent : public ContactMessageContent {
			public:
				ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent(QByteArray const& encryptedVideo, QByteArray const& encryptedImage, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 videoSizeInBytes, quint32 imageSizeInBytes);
				virtual ~ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent();

				virtual ContactMessageContent* clone() const override;

				virtual bool hasPreSendCallbackTask() const override;
				virtual openmittsu::tasks::CallbackTask* getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const override;

				virtual bool hasPostReceiveCallbackTask() const override;
				virtual openmittsu::tasks::CallbackTask* getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const override;

				virtual MessageContent* integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const override;

				virtual QByteArray toPacketPayload() const override;
				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				QByteArray const& getEncryptedVideoData() const;
				QByteArray const& getEncryptedImageData() const;
				openmittsu::crypto::EncryptionKey const& getEncryptionKey() const;
				quint16 getLengthInSeconds() const;
				quint32 getVideoSizeInBytes() const;
				quint32 getImageSizeInBytes() const;
			private:
				QByteArray const m_encryptedVideoData;
				QByteArray const m_encryptedImageData;
				openmittsu::crypto::EncryptionKey const m_encryptionKey;
				quint16 const m_lengthInSeconds;
				quint32 const m_videoSizeInBytes;
				quint32 const m_imageSizeInBytes;
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_CONTACT_CONTACTENCRYPTEDVIDEOANDENCRYPTEDIMAGEANDKEYMESSAGECONTENT_H_
