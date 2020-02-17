#include "src/messages/MessageContent.h"

#include "src/acknowledgments/AcknowledgmentProcessor.h"
#include "src/messages/Message.h"
#include "src/network/ServerConfiguration.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/crypto/BasicCryptoBox.h"
#include "src/utility/HexChar.h"
#include <QByteArray>

namespace openmittsu {
	namespace messages {

		MessageContent::~MessageContent() {
			// Intentionally left empty.
		}

		bool MessageContent::hasPreSendCallbackTask() const {
			return false;
		}

		bool MessageContent::hasPostReceiveCallbackTask() const {
			return false;
		}

		openmittsu::tasks::CallbackTask* MessageContent::getPreSendCallbackTask(Message* preliminaryMessage, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
			return nullptr;
		}

		openmittsu::tasks::CallbackTask* MessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
			return nullptr;
		}

		MessageContent* MessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
			throw;
		}

		void MessageContent::verifyPayloadMinSizeAndSignatureByte(char signatureByte, int minPayloadSize, QByteArray const& payload, bool sizeIsExact) const {
			if (minPayloadSize < 1) {
				minPayloadSize = 1;
			}

			if (payload.size() < minPayloadSize || (sizeIsExact && (payload.size() != minPayloadSize))) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Can not create MessageContent from payload with size " << payload.size() << " instead of " << minPayloadSize << " Bytes. Payload: " << QString(payload.toHex()).toStdString();
			} else if (signatureByte != payload.at(0)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Invalid signatureByte - expected 0x" << openmittsu::utility::HexChar(signatureByte) << " but found 0x" << openmittsu::utility::HexChar(payload.at(0)) << " instead.";
			}
		}

	}
}
