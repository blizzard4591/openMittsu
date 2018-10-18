#include "src/messages/group/GroupAudioIdAndKeyMessageContent.h"

#include "src/utility/Endian.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/messages/MessageContentRegistry.h"
#include "src/messages/group/GroupEncryptedAudioAndKeyMessageContent.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/tasks/BlobDownloaderCallbackTask.h"
#include "src/utility/ByteArrayConversions.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			// Register this MessageContent with the MessageContentRegistry
			bool GroupAudioIdAndKeyMessageContent::m_registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_GROUP_AUDIO, new TypedMessageContentFactory<GroupAudioIdAndKeyMessageContent>());

			GroupAudioIdAndKeyMessageContent::GroupAudioIdAndKeyMessageContent() : GroupMessageContent(openmittsu::protocol::GroupId(0, 0)), m_blobId(), m_encryptionKey(QByteArray(openmittsu::crypto::EncryptionKey::getSizeOfEncryptionKeyInBytes(), 0x00)), m_lengthInSeconds(0), m_sizeInBytes(0) {
				// Only accessible and used by the MessageContentFactory.
			}

			GroupAudioIdAndKeyMessageContent::GroupAudioIdAndKeyMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& blobId, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 sizeInBytes) : GroupMessageContent(groupId), m_blobId(blobId), m_encryptionKey(encryptionKey), m_lengthInSeconds(lengthInSeconds), m_sizeInBytes(sizeInBytes) {
				if (m_blobId.size() != (PROTO_IMAGESERVER_ID_LENGTH_BYTES)) {
					throw openmittsu::exceptions::IllegalArgumentException() << "The supplied blob ID has " << m_blobId.size() << " Bytes instead of " << (PROTO_IMAGESERVER_ID_LENGTH_BYTES) << " Bytes.";
				}
			}

			GroupAudioIdAndKeyMessageContent::~GroupAudioIdAndKeyMessageContent() {
				// Intentionally left empty.
			}

			GroupMessageContent* GroupAudioIdAndKeyMessageContent::clone() const {
				return new GroupAudioIdAndKeyMessageContent(getGroupId(), m_blobId, m_encryptionKey, m_lengthInSeconds, m_sizeInBytes);
			}

			bool GroupAudioIdAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupAudioIdAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobDownloaderCallbackTask(serverConfiguration, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_blobId);
			}

			MessageContent* GroupAudioIdAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobDownloaderCallbackTask const* bdct = dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask);
					if (bdct->getDownloadedBlob().size() != static_cast<int>(m_sizeInBytes)) {
						LOGGER()->warn("Size of downloaded blob differs from stated size ({} Bytes downloaded vs. {} Bytes promised).", bdct->getDownloadedBlob().size(), m_sizeInBytes);
					}

					LOGGER_DEBUG("Integrating result from BlobDownloaderCallbackTask into a new GroupEncryptedAudioAndKeyMessageContent.");
					return new GroupEncryptedAudioAndKeyMessageContent(getGroupId(), bdct->getDownloadedBlob(), m_encryptionKey, m_lengthInSeconds, m_sizeInBytes);
				} else {
					LOGGER()->critical("GroupAudioIdAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& GroupAudioIdAndKeyMessageContent::getBlobId() const {
				return m_blobId;
			}

			openmittsu::crypto::EncryptionKey const& GroupAudioIdAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			quint16 GroupAudioIdAndKeyMessageContent::getLengthInSeconds() const {
				return m_lengthInSeconds;
			}

			quint32 GroupAudioIdAndKeyMessageContent::getSizeInBytes() const {
				return m_sizeInBytes;
			}

			MessageContent* GroupAudioIdAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_GROUP_AUDIO, 1 + openmittsu::protocol::GroupId::getSizeOfGroupIdInBytes() + 2 + (PROTO_IMAGESERVER_ID_LENGTH_BYTES)+ 4 + openmittsu::crypto::EncryptionKey::getSizeOfEncryptionKeyInBytes(), payload, true);

				int startingPosition = 1;
				openmittsu::protocol::GroupId const group(openmittsu::protocol::GroupId::fromData(payload.mid(startingPosition, openmittsu::protocol::GroupId::getSizeOfGroupIdInBytes())));
				startingPosition += openmittsu::protocol::GroupId::getSizeOfGroupIdInBytes();
				uint16_t seconds = openmittsu::utility::ByteArrayConversions::convert2ByteQByteArrayToQuint16(payload.mid(startingPosition, 2));
				startingPosition += 2;
				QByteArray const id(payload.mid(startingPosition, PROTO_IMAGESERVER_ID_LENGTH_BYTES));
				startingPosition += PROTO_IMAGESERVER_ID_LENGTH_BYTES;
				uint32_t const size = openmittsu::utility::ByteArrayConversions::convert4ByteQByteArrayToQuint32(payload.mid(startingPosition, 4));
				startingPosition += 4;
				openmittsu::crypto::EncryptionKey const key(payload.mid(startingPosition, openmittsu::crypto::EncryptionKey::getSizeOfEncryptionKeyInBytes()));

				return new GroupAudioIdAndKeyMessageContent(group, id, key, seconds, size);
			}

			QByteArray GroupAudioIdAndKeyMessageContent::toPacketPayload() const {
				QByteArray result(1, PROTO_MESSAGE_SIGNATURE_GROUP_AUDIO);
				result.append(getGroupId().getGroupIdAsByteArray());
				result.append(openmittsu::utility::Endian::uint16FromHostToLittleEndianByteArray(m_lengthInSeconds));
				result.append(m_blobId);
				result.append(openmittsu::utility::Endian::uint32FromHostToLittleEndianByteArray(m_sizeInBytes));
				result.append(m_encryptionKey.getEncryptionKey());

				return result;
			}

		}
	}
}
