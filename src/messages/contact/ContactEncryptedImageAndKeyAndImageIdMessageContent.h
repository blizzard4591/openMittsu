#ifndef OPENMITTSU_MESSAGES_CONTACT_CONTACTENCRYPTEDIMAGEANDKEYANDIMAGEIDMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_CONTACT_CONTACTENCRYPTEDIMAGEANDKEYANDIMAGEIDMESSAGECONTENT_H_

#include "messages/contact/ContactMessageContent.h"
#include "protocol/Nonce.h"

#include <QByteArray>

class ContactEncryptedImageAndKeyAndImageIdMessageContent : public ContactMessageContent {
public:
	ContactEncryptedImageAndKeyAndImageIdMessageContent(QByteArray const& encryptedImage, Nonce const& imageNonce, quint32 imageSizeInBytes, QByteArray const& blobId);
	virtual ~ContactEncryptedImageAndKeyAndImageIdMessageContent();

	virtual ContactMessageContent* clone() const override;

	virtual bool hasPostReceiveCallbackTask() const override;
	virtual CallbackTask* getPostReceiveCallbackTask(Message* message, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const override;
	virtual MessageContent* integrateCallbackTaskResult(CallbackTask const* callbackTask) const override;

	virtual QByteArray toPacketPayload() const override;
	virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

	QByteArray const& getEncryptedImageData() const;
	Nonce const& getNonce() const;
	quint32 getImageSizeInBytes() const;
	QByteArray const& getImageId() const;
private:
	QByteArray const encryptedImageData;
	Nonce const nonce;
	quint32 const sizeInBytes;
	QByteArray const blobId;
};

#endif // OPENMITTSU_MESSAGES_CONTACT_CONTACTENCRYPTEDIMAGEANDKEYANDIMAGEIDMESSAGECONTENT_H_
