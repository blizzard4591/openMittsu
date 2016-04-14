#ifndef OPENMITTSU_MESSAGES_CONTACT_CONTACTIMAGEIDANDKEYMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_CONTACT_CONTACTIMAGEIDANDKEYMESSAGECONTENT_H_

#include "messages/MessageContentFactory.h"
#include "messages/contact/ContactMessageContent.h"
#include "protocol/Nonce.h"

#include <QByteArray>
#include <QtGlobal>

class ContactImageIdAndKeyMessageContent : public ContactMessageContent {
public:
	ContactImageIdAndKeyMessageContent(QByteArray const& imageId, Nonce const& imageNonce, quint32 sizeInBytes);
	virtual ~ContactImageIdAndKeyMessageContent();

	virtual ContactMessageContent* clone() const override;

	virtual bool hasPostReceiveCallbackTask() const;

	virtual CallbackTask* getPostReceiveCallbackTask(Message* message, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const override;
	virtual MessageContent* integrateCallbackTaskResult(CallbackTask const* callbackTask) const override;

	virtual QByteArray toPacketPayload() const override;
	virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

	QByteArray const& getImageId() const;
	Nonce const& getImageNonce() const;
	quint32 getImageSizeInBytes() const;

	friend class TypedMessageContentFactory<ContactImageIdAndKeyMessageContent>;
private:
	QByteArray imageId;
	Nonce imageNonce;
	quint32 sizeInBytes;

	static bool registrationResult;

	ContactImageIdAndKeyMessageContent();
};

#endif // OPENMITTSU_MESSAGES_CONTACT_CONTACTIMAGEIDANDKEYMESSAGECONTENT_H_
