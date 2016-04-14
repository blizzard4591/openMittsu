#ifndef OPENMITTSU_MESSAGES_MESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_MESSAGECONTENT_H_

#include "acknowledgments/AcknowledgmentProcessor.h"
#include "messages/FullMessageHeader.h"

#include <memory>

class Message;
class CallbackTask;
class QByteArray;
class CryptoBox;
class ServerConfiguration;

class MessageContent {
public:
	virtual ~MessageContent();

	virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const = 0;
	virtual QByteArray toPacketPayload() const = 0;

	virtual bool hasPreSendCallbackTask() const;
	virtual bool hasPostReceiveCallbackTask() const;

	virtual CallbackTask* getPreSendCallbackTask(Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const;
	virtual CallbackTask* getPostReceiveCallbackTask(Message* message, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const;

	virtual MessageContent* integrateCallbackTaskResult(CallbackTask const* callbackTask) const;
protected:
	void verifyPayloadMinSizeAndSignatureByte(char signatureByte, int minPayloadSize, QByteArray const& payload, bool sizeIsExact = false) const;
};

#endif // OPENMITTSU_MESSAGES_MESSAGECONTENT_H_
