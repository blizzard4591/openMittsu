#include "src/messages/group/GroupSyncMessageContent.h"

#include "src/messages/MessageContentRegistry.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/ByteArrayConversions.h"

#include <memory>

namespace openmittsu {
	namespace messages {
		namespace group {

			// Register this MessageContent with the MessageContentRegistry
			bool GroupSyncMessageContent::registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_GROUP_SYNC, std::make_shared<TypedMessageContentFactory<GroupSyncMessageContent>>());


			GroupSyncMessageContent::GroupSyncMessageContent() : GroupMessageContent(openmittsu::protocol::GroupId(0, 0)) {
				// Only accessible and used by the MessageContentFactory.
			}

			GroupSyncMessageContent::GroupSyncMessageContent(openmittsu::protocol::GroupId const& groupId) : GroupMessageContent(groupId) {
				// Intentionally left empty.
			}

			GroupSyncMessageContent::~GroupSyncMessageContent() {
				// Intentionally left empty.
			}

			GroupMessageContent* GroupSyncMessageContent::clone() const {
				return new GroupSyncMessageContent(getGroupId());
			}

			MessageContent* GroupSyncMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				int const pureGroupIdSize = openmittsu::protocol::GroupId::getSizeOfGroupIdInBytes() - openmittsu::protocol::ContactId::getSizeOfContactIdInBytes();
				verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_GROUP_SYNC, 1 + pureGroupIdSize, payload, true);

				quint64 const pureGroupId = openmittsu::utility::ByteArrayConversions::convert8ByteQByteArrayToQuint64(payload.mid(1, pureGroupIdSize));

				openmittsu::protocol::GroupId const groupId(messageHeader.getReceiver(), pureGroupId);

				return new GroupSyncMessageContent(groupId);
			}

			QByteArray GroupSyncMessageContent::toPacketPayload() const {
				QByteArray result(1, PROTO_MESSAGE_SIGNATURE_GROUP_SYNC);

				quint64 const pureGroupId = getGroupId().getGroupId();
				QByteArray const pureGroupIdBytes = openmittsu::utility::ByteArrayConversions::convertQuint64toQByteArray(pureGroupId);

				result.append(pureGroupIdBytes);

				return result;
			}

		}
	}
}
