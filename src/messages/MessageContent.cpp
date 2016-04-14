#include "messages/MessageContent.h"

#include "messages/Message.h"
#include "ServerConfiguration.h"
#include "exceptions/IllegalArgumentException.h"
#include "protocol/CryptoBox.h"
#include "utility/HexChar.h"
#include <QByteArray>

MessageContent::~MessageContent() {
	// Intentionally left empty.
}

bool MessageContent::hasPreSendCallbackTask() const {
	return false;
}

bool MessageContent::hasPostReceiveCallbackTask() const {
	return false;
}

CallbackTask* MessageContent::getPreSendCallbackTask(Message* preliminaryMessage, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const {
	return nullptr;
}

CallbackTask* MessageContent::getPostReceiveCallbackTask(Message* message, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const {
	return nullptr;
}

MessageContent* MessageContent::integrateCallbackTaskResult(CallbackTask const* callbackTask) const {
	throw;
}

void MessageContent::verifyPayloadMinSizeAndSignatureByte(char signatureByte, int minPayloadSize, QByteArray const& payload, bool sizeIsExact) const {
	if (minPayloadSize < 1) {
		minPayloadSize = 1;
	}

	if (payload.size() < minPayloadSize || (sizeIsExact && (payload.size() != minPayloadSize))) {
		throw IllegalArgumentException() << "Can not create MessageContent from payload with size " << payload.size() << " instead of " << minPayloadSize << " Bytes.";
	} else if (signatureByte != payload.at(0)) {
		throw IllegalArgumentException() << "Invalid signatureByte - expected 0x" << HexChar(signatureByte) << " but found 0x" << HexChar(payload.at(0)) << " instead.";
	}
}
