#ifndef OPENMITTSU_MESSAGES_CONTACT_CONTACTENCRYPTEDIMAGEANDKEYMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_CONTACT_CONTACTENCRYPTEDIMAGEANDKEYMESSAGECONTENT_H_

#include "messages/contact/ContactMessageContent.h"
#include "protocol/Nonce.h"

#include <QByteArray>

class ContactEncryptedImageAndKeyMessageContent : public ContactMessageContent {
public:
	ContactEncryptedImageAndKeyMessageContent(QByteArray const& encryptedImage, Nonce const& imageNonce, quint32 imageSizeInBytes);
	virtual ~ContactEncryptedImageAndKeyMessageContent();

	virtual ContactMessageContent* clone() const override;

	virtual bool hasPreSendCallbackTask() const override;
	virtual CallbackTask* getPreSendCallbackTask(Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const override;

	virtual bool hasPostReceiveCallbackTask() const override;
	virtual CallbackTask* getPostReceiveCallbackTask(Message* message, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const override;
	
	virtual MessageContent* integrateCallbackTaskResult(CallbackTask const* callbackTask) const override;

	virtual QByteArray toPacketPayload() const override;
	virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

	QByteArray const& getEncryptedImageData() const;
	Nonce const& getNonce() const;
	quint32 getImageSizeInBytes() const;
private:
	QByteArray const encryptedImageData;
	Nonce const nonce;
	quint32 const sizeInBytes;
};

#endif // OPENMITTSU_MESSAGES_CONTACT_CONTACTENCRYPTEDIMAGEANDKEYMESSAGECONTENT_H_
