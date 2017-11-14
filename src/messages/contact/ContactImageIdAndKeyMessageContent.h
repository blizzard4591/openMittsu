#ifndef OPENMITTSU_MESSAGES_CONTACT_CONTACTIMAGEIDANDKEYMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_CONTACT_CONTACTIMAGEIDANDKEYMESSAGECONTENT_H_

#include "src/messages/MessageContentFactory.h"
#include "src/messages/contact/ContactMessageContent.h"
#include "src/crypto/Nonce.h"

#include <QByteArray>
#include <QtGlobal>

namespace openmittsu {
	namespace messages {
		namespace contact {

			class ContactImageIdAndKeyMessageContent : public ContactMessageContent {
			public:
				ContactImageIdAndKeyMessageContent(QByteArray const& imageId, openmittsu::crypto::Nonce const& imageNonce, quint32 sizeInBytes);
				virtual ~ContactImageIdAndKeyMessageContent();

				virtual ContactMessageContent* clone() const override;

				virtual bool hasPostReceiveCallbackTask() const override;

				virtual openmittsu::tasks::CallbackTask* getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const override;
				virtual MessageContent* integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const override;

				virtual QByteArray toPacketPayload() const override;
				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				QByteArray const& getImageId() const;
				openmittsu::crypto::Nonce const& getImageNonce() const;
				quint32 getImageSizeInBytes() const;

				friend class TypedMessageContentFactory<ContactImageIdAndKeyMessageContent>;
			private:
				QByteArray imageId;
				openmittsu::crypto::Nonce imageNonce;
				quint32 sizeInBytes;

				static bool registrationResult;

				ContactImageIdAndKeyMessageContent();
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_CONTACT_CONTACTIMAGEIDANDKEYMESSAGECONTENT_H_
