#include "src/messages/group/GroupCreationMessageContent.h"

#include "src/exceptions/ProtocolErrorException.h"
#include "src/messages/MessageContentRegistry.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/ByteArrayConversions.h"

#include <memory>

namespace openmittsu {
	namespace messages {
		namespace group {

			// Register this MessageContent with the MessageContentRegistry
			bool GroupCreationMessageContent::registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_GROUP_CREATION, std::make_shared<TypedMessageContentFactory<GroupCreationMessageContent>>());


			GroupCreationMessageContent::GroupCreationMessageContent() : GroupMessageContent(openmittsu::protocol::GroupId(0, 0)) {
				// Only accessible and used by the MessageContentFactory.
			}

			GroupCreationMessageContent::GroupCreationMessageContent(openmittsu::protocol::GroupId const& groupId, QSet<openmittsu::protocol::ContactId> const& groupMembers) : GroupMessageContent(groupId), members(groupMembers) {
				// Intentionally left empty.
			}

			GroupCreationMessageContent::~GroupCreationMessageContent() {
				// Intentionally left empty.
			}

			GroupMessageContent* GroupCreationMessageContent::clone() const {
				return new GroupCreationMessageContent(getGroupId(), members);
			}

			QSet<openmittsu::protocol::ContactId> const& GroupCreationMessageContent::getGroupMembers() const {
				return members;
			}

			MessageContent* GroupCreationMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				int const pureGroupIdSize = openmittsu::protocol::GroupId::getSizeOfGroupIdInBytes() - openmittsu::protocol::ContactId::getSizeOfContactIdInBytes();
				verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_GROUP_CREATION, 1 + pureGroupIdSize, payload);

				quint64 const pureGroupId = openmittsu::utility::ByteArrayConversions::convert8ByteQByteArrayToQuint64(payload.mid(1, pureGroupIdSize));

				openmittsu::protocol::GroupId const groupId(messageHeader.getSender(), pureGroupId);

				QByteArray const memberIds = payload.mid(1 + pureGroupIdSize);
				if ((memberIds.size() % openmittsu::protocol::ContactId::getSizeOfContactIdInBytes()) != 0) {
					throw openmittsu::exceptions::ProtocolErrorException() << "Can not decode Group Creation Message where the member id list size is not a multiple of the size of a single ID. Payload: " << QString(payload.toHex()).toStdString();
				}

				int const memberCount = memberIds.size() / openmittsu::protocol::ContactId::getSizeOfContactIdInBytes();
				QSet<openmittsu::protocol::ContactId> groupMembers;
				for (int i = 0; i < memberCount; ++i) {
					openmittsu::protocol::ContactId const memberId(memberIds.mid(i * openmittsu::protocol::ContactId::getSizeOfContactIdInBytes(), openmittsu::protocol::ContactId::getSizeOfContactIdInBytes()));
					groupMembers.insert(memberId);
				}

				return new GroupCreationMessageContent(groupId, groupMembers);
			}

			QByteArray GroupCreationMessageContent::toPacketPayload() const {
				QByteArray result(1, PROTO_MESSAGE_SIGNATURE_GROUP_CREATION);

				quint64 const pureGroupId = getGroupId().getGroupId();
				QByteArray const pureGroupIdBytes = openmittsu::utility::ByteArrayConversions::convertQuint64toQByteArray(pureGroupId);

				result.append(pureGroupIdBytes);

				QSet<openmittsu::protocol::ContactId>::const_iterator it = members.constBegin();
				QSet<openmittsu::protocol::ContactId>::const_iterator end = members.constEnd();
				for (; it != end; ++it) {
					result.append(it->getContactIdAsByteArray());
				}

				return result;
			}

		}
	}
}
