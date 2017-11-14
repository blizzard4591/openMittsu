#ifndef OPENMITTSU_MESSAGES_CONTACT_CONTACTIMAGEMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_CONTACT_CONTACTIMAGEMESSAGECONTENT_H_

#include "src/messages/contact/ContactMessageContent.h"

#include <QByteArray>

namespace openmittsu {
	namespace messages {
		namespace contact {

			class ContactImageMessageContent : public ContactMessageContent {
			public:
				ContactImageMessageContent(QByteArray const& imageData);
				virtual ~ContactImageMessageContent();

				virtual ContactMessageContent* clone() const override;

				virtual bool hasPreSendCallbackTask() const override;

				virtual openmittsu::tasks::CallbackTask* getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const override;
				virtual MessageContent* integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const override;

				virtual QByteArray toPacketPayload() const override;
				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				QByteArray const& getImageData() const;
			private:
				QByteArray imageData;

				static bool registrationResult;
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_CONTACT_CONTACTIMAGEMESSAGECONTENT_H_
