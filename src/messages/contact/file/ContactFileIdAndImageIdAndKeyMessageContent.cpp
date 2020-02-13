#include "src/messages/contact/file/ContactFileIdAndImageIdAndKeyMessageContent.h"

#include "src/utility/Endian.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/messages/MessageContentRegistry.h"
#include "src/messages/contact/file/ContactEncryptedFileAndImageIdAndKeyMessageContent.h"
#include "src/messages/contact/file/ContactEncryptedFileAndImageAndKeyMessageContent.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/tasks/BlobDownloaderCallbackTask.h"
#include "src/utility/ByteArrayConversions.h"
#include "src/utility/Logging.h"

#include <memory>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

namespace openmittsu {
	namespace messages {
		namespace contact {

			// Register this MessageContent with the MessageContentRegistry
			bool ContactFileIdAndImageIdAndKeyMessageContent::m_registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_CONTACT_FILE, std::make_shared<TypedMessageContentFactory<ContactFileIdAndImageIdAndKeyMessageContent>>());

			ContactFileIdAndImageIdAndKeyMessageContent::ContactFileIdAndImageIdAndKeyMessageContent() : ContactMessageContent(), m_fileBlobId(), m_imageBlobId(), m_encryptionKey(QByteArray(openmittsu::crypto::EncryptionKey::getSizeOfEncryptionKeyInBytes(), 0x00)), m_mimeType(""), m_fileName(""), m_caption(""), m_fileSizeInBytes(0) {
				// Only accessible and used by the MessageContentFactory.
			}

			ContactFileIdAndImageIdAndKeyMessageContent::ContactFileIdAndImageIdAndKeyMessageContent(QByteArray const& fileBlobId, QByteArray const& imageBlobId, openmittsu::crypto::EncryptionKey const& encryptionKey, QString const& mimeType, QString const& fileName, QString const& caption, quint64 fileSizeInBytes) : ContactMessageContent(), m_fileBlobId(fileBlobId), m_imageBlobId(imageBlobId), m_encryptionKey(encryptionKey), m_mimeType(mimeType), m_fileName(fileName), m_caption(caption), m_fileSizeInBytes(fileSizeInBytes) {
				if (m_fileBlobId.size() != (PROTO_IMAGESERVER_ID_LENGTH_BYTES)) {
					throw openmittsu::exceptions::IllegalArgumentException() << "The supplied blob ID has " << m_fileBlobId.size() << " Bytes instead of " << (PROTO_IMAGESERVER_ID_LENGTH_BYTES) << " Bytes.";
				}
				if (m_imageBlobId.size() != (PROTO_IMAGESERVER_ID_LENGTH_BYTES)) {
					throw openmittsu::exceptions::IllegalArgumentException() << "The supplied blob ID has " << m_imageBlobId.size() << " Bytes instead of " << (PROTO_IMAGESERVER_ID_LENGTH_BYTES) << " Bytes.";
				}
			}

			ContactFileIdAndImageIdAndKeyMessageContent::~ContactFileIdAndImageIdAndKeyMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactFileIdAndImageIdAndKeyMessageContent::clone() const {
				return new ContactFileIdAndImageIdAndKeyMessageContent(m_fileBlobId, m_imageBlobId, m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
			}

			bool ContactFileIdAndImageIdAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactFileIdAndImageIdAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobDownloaderCallbackTask(serverConfiguration, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_fileBlobId);
			}

			MessageContent* ContactFileIdAndImageIdAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobDownloaderCallbackTask const* bdct = dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask);
					if (bdct->getDownloadedBlob().size() != static_cast<int>(m_fileSizeInBytes)) {
						LOGGER()->warn("Size of downloaded blob differs from stated size ({} Bytes downloaded vs. {} Bytes promised).", bdct->getDownloadedBlob().size(), m_fileSizeInBytes);
					}

					LOGGER_DEBUG("Integrating result from BlobDownloaderCallbackTask into a new ContactEncryptedVideoAndImageIdAndKeyMessageContent.");
					if (m_imageBlobId.isEmpty()) {
						return new ContactEncryptedFileAndImageAndKeyMessageContent(bdct->getDownloadedBlob(), QByteArray(), m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
					}
					return new ContactEncryptedFileAndImageIdAndKeyMessageContent(bdct->getDownloadedBlob(), m_imageBlobId, m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
				} else {
					LOGGER()->critical("ContactFileIdAndImageIdAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& ContactFileIdAndImageIdAndKeyMessageContent::getFileBlobId() const {
				return m_fileBlobId;
			}

			QByteArray const& ContactFileIdAndImageIdAndKeyMessageContent::getImageBlobId() const {
				return m_imageBlobId;
			}

			openmittsu::crypto::EncryptionKey const& ContactFileIdAndImageIdAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			QString const& ContactFileIdAndImageIdAndKeyMessageContent::getMimeType() const {
				return m_mimeType;
			}

			QString const& ContactFileIdAndImageIdAndKeyMessageContent::getFileName() const {
				return m_fileName;
			}

			QString const& ContactFileIdAndImageIdAndKeyMessageContent::getCaption() const {
				return m_caption;
			}

			quint64 ContactFileIdAndImageIdAndKeyMessageContent::getFileSizeInBytes() const {
				return m_fileSizeInBytes;
			}

			MessageContent* ContactFileIdAndImageIdAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_CONTACT_FILE, 1 + 140, payload, false);
				// 144 is about the minimal size the json can have, so we use 140 to safely approximate

				int startingPosition = 1;
				QByteArray const jsonData(payload.mid(startingPosition));

				QJsonParseError jsonParseError;
				QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData, &jsonParseError);
				if (jsonDocument.isNull()) {
					LOGGER()->error("Failed to parse JSON received in contact file message: {}", QString(jsonData.toHex()).toStdString());
					return nullptr;
				} else if (!jsonDocument.isObject()) {
					LOGGER()->error("Failed to parse JSON received in contact file message, root is not an object: {}", QString(jsonData.toHex()).toStdString());
					return nullptr;
				}

				QJsonObject const jsonRootObject = jsonDocument.object();
				if (!jsonRootObject.contains("b") || !jsonRootObject["b"].isString()) {
					LOGGER()->error("Failed to parse JSON received in contact file message, 'b' is missing or no string: {}", QString(jsonData.toHex()).toStdString());
					return nullptr;
				} else if (!jsonRootObject.contains("k") || !jsonRootObject["k"].isString()) {
					LOGGER()->error("Failed to parse JSON received in contact file message, 'k' is missing or no string: {}", QString(jsonData.toHex()).toStdString());
					return nullptr;
				} else if (!jsonRootObject.contains("m") || !jsonRootObject["m"].isString()) {
					LOGGER()->error("Failed to parse JSON received in contact file message, 'm' is missing or no string: {}", QString(jsonData.toHex()).toStdString());
					return nullptr;
				} else if (!jsonRootObject.contains("n") || !jsonRootObject["n"].isString()) {
					LOGGER()->error("Failed to parse JSON received in contact file message, 'n' is missing or no string: {}", QString(jsonData.toHex()).toStdString());
					return nullptr;
				} else if (!jsonRootObject.contains("s") || !jsonRootObject["s"].isDouble()) {
					LOGGER()->error("Failed to parse JSON received in contact file message, 's' is missing or no string: {}", QString(jsonData.toHex()).toStdString());
					return nullptr;
				} else if (!jsonRootObject.contains("i") || !jsonRootObject["i"].isDouble()) {
					LOGGER()->error("Failed to parse JSON received in contact file message, 'i' is missing or no string: {}", QString(jsonData.toHex()).toStdString());
					return nullptr;
				}

				QByteArray const fileId(QByteArray::fromHex(jsonRootObject["b"].toString().toUtf8()));
				
				QByteArray thumbId;
				if (jsonRootObject.contains("t") || jsonRootObject["t"].isString()) {
					thumbId = QByteArray::fromHex(jsonRootObject["t"].toString().toUtf8());
				}

				openmittsu::crypto::EncryptionKey const key(QByteArray::fromHex(jsonRootObject["k"].toString().toUtf8()));
				QString const mimeType = jsonRootObject["m"].toString();
				QString const fileName = jsonRootObject["n"].toString();
				quint64 const fileSizeInBytes = jsonRootObject["s"].toInt();

				QString caption("");
				if (jsonRootObject.contains("d") && jsonRootObject["d"].isString()) {
					caption = jsonRootObject["d"].toString();
				}

				return new ContactFileIdAndImageIdAndKeyMessageContent(fileId, thumbId, key, mimeType, fileName, caption, fileSizeInBytes);
			}

			QByteArray ContactFileIdAndImageIdAndKeyMessageContent::toPacketPayload() const {
				QByteArray result(1, PROTO_MESSAGE_SIGNATURE_CONTACT_FILE);

				QJsonObject rootObject;
				rootObject["b"] = QString(m_fileBlobId.toHex());
				if (!m_imageBlobId.isEmpty()) {
					rootObject["t"] = QString(m_imageBlobId.toHex());
				}
				rootObject["k"] = QString(m_encryptionKey.getEncryptionKey().toHex());
				rootObject["m"] = m_mimeType;
				rootObject["n"] = m_fileName;
				rootObject["s"] = (int)m_fileSizeInBytes;
				rootObject["i"] = 1; // Unknown Field
				if (!m_caption.isEmpty()) {
					rootObject["d"] = m_caption;
				}

				QJsonDocument jsonDocument(rootObject);
				QByteArray const jsonData(jsonDocument.toJson());
				result.append(jsonData);

				return result;
			}

		}
	}
}
