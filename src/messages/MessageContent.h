#ifndef OPENMITTSU_MESSAGES_MESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_MESSAGECONTENT_H_

#include "src/messages/FullMessageHeader.h"

#include <QByteArray>

#include <memory>

namespace openmittsu {
	namespace acknowledgments {
		class AcknowledgmentProcessor;
	}
	namespace crypto {
		class FullCryptoBox;
	}
	namespace messages {
		class Message;
	}
	namespace network {
		class ServerConfiguration;
	}
	namespace tasks {
		class CallbackTask;
	}

	namespace messages {

		class MessageContent {
		public:
			virtual ~MessageContent();

			virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const = 0;
			virtual QByteArray toPacketPayload() const = 0;

			virtual bool hasPreSendCallbackTask() const;
			virtual bool hasPostReceiveCallbackTask() const;

			virtual openmittsu::tasks::CallbackTask* getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const;
			virtual openmittsu::tasks::CallbackTask* getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const;

			virtual MessageContent* integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const;
		protected:
			void verifyPayloadMinSizeAndSignatureByte(char signatureByte, int minPayloadSize, QByteArray const& payload, bool sizeIsExact = false) const;
		};

	}
}

#endif // OPENMITTSU_MESSAGES_MESSAGECONTENT_H_
