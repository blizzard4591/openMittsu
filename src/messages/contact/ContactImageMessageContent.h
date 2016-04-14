#ifndef OPENMITTSU_MESSAGES_CONTACT_CONTACTIMAGEMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_CONTACT_CONTACTIMAGEMESSAGECONTENT_H_

#include "messages/contact/ContactMessageContent.h"

#include <QByteArray>

class ContactImageMessageContent : public ContactMessageContent {
public:
	ContactImageMessageContent(QByteArray const& imageData);
	virtual ~ContactImageMessageContent();

	virtual ContactMessageContent* clone() const override;

	virtual bool hasPreSendCallbackTask() const override;

	virtual CallbackTask* getPreSendCallbackTask(Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const override;
	virtual MessageContent* integrateCallbackTaskResult(CallbackTask const* callbackTask) const override;

	virtual QByteArray toPacketPayload() const override;
	virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

	QByteArray const& getImageData() const;
private:
	QByteArray imageData;

	static bool registrationResult;
};

#endif // OPENMITTSU_MESSAGES_CONTACT_CONTACTIMAGEMESSAGECONTENT_H_
