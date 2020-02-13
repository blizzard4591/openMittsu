#include "src/messages/group/file/GroupFileIdAndImageIdAndKeyMessageContent.h"

#include "src/utility/Endian.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/messages/MessageContentRegistry.h"
#include "src/messages/group/file/GroupEncryptedFileAndImageIdAndKeyMessageContent.h"
#include "src/messages/group/file/GroupEncryptedFileAndImageAndKeyMessageContent.h"
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
		namespace group {

			// Register this MessageContent with the MessageContentRegistry
			bool GroupFileIdAndImageIdAndKeyMessageContent::m_registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_GROUP_FILE, std::make_shared<TypedMessageContentFactory<GroupFileIdAndImageIdAndKeyMessageContent>>());

			GroupFileIdAndImageIdAndKeyMessageContent::GroupFileIdAndImageIdAndKeyMessageContent() : GroupMessageContent(openmittsu::protocol::GroupId(0, 0)), m_fileBlobId(), m_imageBlobId(), m_encryptionKey(QByteArray(openmittsu::crypto::EncryptionKey::getSizeOfEncryptionKeyInBytes(), 0x00)), m_mimeType(""), m_fileName(""), m_caption(""), m_fileSizeInBytes(0) {
				// Only accessible and used by the MessageContentFactory.
			}

			GroupFileIdAndImageIdAndKeyMessageContent::GroupFileIdAndImageIdAndKeyMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& fileBlobId, QByteArray const& imageBlobId, openmittsu::crypto::EncryptionKey const& encryptionKey, QString const& mimeType, QString const& fileName, QString const& caption, quint64 fileSizeInBytes) : GroupMessageContent(groupId), m_fileBlobId(fileBlobId), m_imageBlobId(imageBlobId), m_encryptionKey(encryptionKey), m_mimeType(mimeType), m_fileName(fileName), m_caption(caption), m_fileSizeInBytes(fileSizeInBytes) {
				if (m_fileBlobId.size() != (PROTO_IMAGESERVER_ID_LENGTH_BYTES)) {
					throw openmittsu::exceptions::IllegalArgumentException() << "The supplied blob ID has " << m_fileBlobId.size() << " Bytes instead of " << (PROTO_IMAGESERVER_ID_LENGTH_BYTES) << " Bytes.";
				}
				if (m_imageBlobId.size() != (PROTO_IMAGESERVER_ID_LENGTH_BYTES)) {
					throw openmittsu::exceptions::IllegalArgumentException() << "The supplied blob ID has " << m_imageBlobId.size() << " Bytes instead of " << (PROTO_IMAGESERVER_ID_LENGTH_BYTES) << " Bytes.";
				}
			}

			GroupFileIdAndImageIdAndKeyMessageContent::~GroupFileIdAndImageIdAndKeyMessageContent() {
				// Intentionally left empty.
			}

			GroupMessageContent* GroupFileIdAndImageIdAndKeyMessageContent::clone() const {
				return new GroupFileIdAndImageIdAndKeyMessageContent(getGroupId(), m_fileBlobId, m_imageBlobId, m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
			}

			bool GroupFileIdAndImageIdAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupFileIdAndImageIdAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobDownloaderCallbackTask(serverConfiguration, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_fileBlobId);
			}

			MessageContent* GroupFileIdAndImageIdAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobDownloaderCallbackTask const* bdct = dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask);
					if (bdct->getDownloadedBlob().size() != static_cast<int>(m_fileSizeInBytes)) {
						LOGGER()->warn("Size of downloaded blob differs from stated size ({} Bytes downloaded vs. {} Bytes promised).", bdct->getDownloadedBlob().size(), m_fileSizeInBytes);
					}

					LOGGER_DEBUG("Integrating result from BlobDownloaderCallbackTask into a new GroupEncryptedVideoAndImageIdAndKeyMessageContent.");
					if (m_imageBlobId.isEmpty()) {
						return new GroupEncryptedFileAndImageAndKeyMessageContent(getGroupId(), bdct->getDownloadedBlob(), QByteArray(), m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
					}
					return new GroupEncryptedFileAndImageIdAndKeyMessageContent(getGroupId(), bdct->getDownloadedBlob(), m_imageBlobId, m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
				} else {
					LOGGER()->critical("GroupFileIdAndImageIdAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& GroupFileIdAndImageIdAndKeyMessageContent::getFileBlobId() const {
				return m_fileBlobId;
			}

			QByteArray const& GroupFileIdAndImageIdAndKeyMessageContent::getImageBlobId() const {
				return m_imageBlobId;
			}

			openmittsu::crypto::EncryptionKey const& GroupFileIdAndImageIdAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			QString const& GroupFileIdAndImageIdAndKeyMessageContent::getMimeType() const {
				return m_mimeType;
			}

			QString const& GroupFileIdAndImageIdAndKeyMessageContent::getFileName() const {
				return m_fileName;
			}

			QString const& GroupFileIdAndImageIdAndKeyMessageContent::getCaption() const {
				return m_caption;
			}

			quint64 GroupFileIdAndImageIdAndKeyMessageContent::getFileSizeInBytes() const {
				return m_fileSizeInBytes;
			}

			MessageContent* GroupFileIdAndImageIdAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_GROUP_FILE, 1 + 16 + 140, payload, false);
				// 144 is about the minimal size the json can have, so we use 140 to safely approximate

				
				int startingPosition = 1;
				openmittsu::protocol::GroupId const group(openmittsu::protocol::GroupId::fromData(payload.mid(startingPosition, openmittsu::protocol::GroupId::getSizeOfGroupIdInBytes())));
				startingPosition += openmittsu::protocol::GroupId::getSizeOfGroupIdInBytes();

				QByteArray const jsonData(payload.mid(startingPosition));

				QJsonParseError jsonParseError;
				QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData, &jsonParseError);
				if (jsonDocument.isNull()) {
					LOGGER()->error("Failed to parse JSON received in group file message: {}", QString(jsonData.toHex()).toStdString());
					return nullptr;
				} else if (!jsonDocument.isObject()) {
					LOGGER()->error("Failed to parse JSON received in group file message, root is not an object: {}", QString(jsonData.toHex()).toStdString());
					return nullptr;
				}

				QJsonObject const jsonRootObject = jsonDocument.object();
				if (!jsonRootObject.contains("b") || !jsonRootObject["b"].isString()) {
					LOGGER()->error("Failed to parse JSON received in group file message, 'b' is missing or no string: {}", QString(jsonData.toHex()).toStdString());
					return nullptr;
				} else if (!jsonRootObject.contains("k") || !jsonRootObject["k"].isString()) {
					LOGGER()->error("Failed to parse JSON received in group file message, 'k' is missing or no string: {}", QString(jsonData.toHex()).toStdString());
					return nullptr;
				} else if (!jsonRootObject.contains("m") || !jsonRootObject["m"].isString()) {
					LOGGER()->error("Failed to parse JSON received in group file message, 'm' is missing or no string: {}", QString(jsonData.toHex()).toStdString());
					return nullptr;
				} else if (!jsonRootObject.contains("n") || !jsonRootObject["n"].isString()) {
					LOGGER()->error("Failed to parse JSON received in group file message, 'n' is missing or no string: {}", QString(jsonData.toHex()).toStdString());
					return nullptr;
				} else if (!jsonRootObject.contains("s") || !jsonRootObject["s"].isDouble()) {
					LOGGER()->error("Failed to parse JSON received in group file message, 's' is missing or no string: {}", QString(jsonData.toHex()).toStdString());
					return nullptr;
				} else if (!jsonRootObject.contains("i") || !jsonRootObject["i"].isDouble()) {
					LOGGER()->error("Failed to parse JSON received in group file message, 'i' is missing or no string: {}", QString(jsonData.toHex()).toStdString());
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

				return new GroupFileIdAndImageIdAndKeyMessageContent(group, fileId, thumbId, key, mimeType, fileName, caption, fileSizeInBytes);
			}

			QByteArray GroupFileIdAndImageIdAndKeyMessageContent::toPacketPayload() const {
				QByteArray result(1, PROTO_MESSAGE_SIGNATURE_GROUP_FILE);

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

				result.append(getGroupId().getGroupIdAsByteArray());
				result.append(jsonData);

				return result;
			}

		}
	}
}
