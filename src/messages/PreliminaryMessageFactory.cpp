#include "src/messages/PreliminaryMessageFactory.h"

#include "src/messages/contact/ContactTextMessageContent.h"
#include "src/messages/contact/ContactImageMessageContent.h"
#include "src/messages/contact/ContactLocationMessageContent.h"
#include "src/messages/contact/PreliminaryContactMessageHeader.h"
#include "src/messages/contact/ReceiptMessageContent.h"
#include "src/messages/contact/UserTypingMessageContent.h"

#include "src/messages/group/PreliminaryGroupMessageHeader.h"
#include "src/messages/group/GroupTextMessageContent.h"
#include "src/messages/group/GroupImageMessageContent.h"
#include "src/messages/group/GroupLocationMessageContent.h"

#include "src/messages/group/GroupCreationMessageContent.h"
#include "src/messages/group/GroupLeaveMessageContent.h"
#include "src/messages/group/GroupSyncMessageContent.h"
#include "src/messages/group/GroupSetPhotoMessageContent.h"
#include "src/messages/group/GroupSetTitleMessageContent.h"

#include "src/messages/MessageFlagsFactory.h"

namespace openmittsu {
	namespace messages {

		PreliminaryMessageFactory::PreliminaryMessageFactory() {
			throw;
		}

		PreliminaryMessageFactory::PreliminaryMessageFactory(PreliminaryMessageFactory const& other) {
			throw;
		}

		PreliminaryMessageFactory::~PreliminaryMessageFactory() {
			// Intentionally left empty.
		}

		contact::PreliminaryContactMessage PreliminaryMessageFactory::createPreliminaryContactTextMessage(openmittsu::protocol::ContactId const& receiverId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, QString const& text) {
			return contact::PreliminaryContactMessage(new contact::PreliminaryContactMessageHeader(receiverId, messageId, time, MessageFlagsFactory::createContactMessageFlags()), new contact::ContactTextMessageContent(text));
		}

		contact::PreliminaryContactMessage PreliminaryMessageFactory::createPreliminaryContactImageMessage(openmittsu::protocol::ContactId const& receiverId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, QByteArray const& imageData) {
			return contact::PreliminaryContactMessage(new contact::PreliminaryContactMessageHeader(receiverId, messageId, time, MessageFlagsFactory::createContactMessageFlags()), new contact::ContactImageMessageContent(imageData));
		}

		contact::PreliminaryContactMessage PreliminaryMessageFactory::createPreliminaryContactLocationMessage(openmittsu::protocol::ContactId const& receiverId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, openmittsu::utility::Location const& location) {
			return contact::PreliminaryContactMessage(new contact::PreliminaryContactMessageHeader(receiverId, messageId, time, MessageFlagsFactory::createContactMessageFlags()), new contact::ContactLocationMessageContent(location.getLatitude(), location.getLongitude(), location.getHeight(), location.getDescription()));
		}

		contact::PreliminaryContactMessage PreliminaryMessageFactory::createPreliminaryContactUserTypingStartedMessage(openmittsu::protocol::ContactId const& receiverId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time) {
			return contact::PreliminaryContactMessage(new contact::PreliminaryContactMessageHeader(receiverId, messageId, time, MessageFlagsFactory::createTypingStatusMessageFlags()), new contact::UserTypingMessageContent(true));
		}

		contact::PreliminaryContactMessage PreliminaryMessageFactory::createPreliminaryContactUserTypingStoppedMessage(openmittsu::protocol::ContactId const& receiverId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time) {
			return contact::PreliminaryContactMessage(new contact::PreliminaryContactMessageHeader(receiverId, messageId, time, MessageFlagsFactory::createTypingStatusMessageFlags()), new contact::UserTypingMessageContent(false));
		}

		contact::PreliminaryContactMessage PreliminaryMessageFactory::createPreliminaryContactMessageReceipt(openmittsu::protocol::ContactId const& receiverId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, openmittsu::protocol::MessageId const& relatedMessage, contact::ReceiptMessageContent::ReceiptType const& receiptType) {
			return contact::PreliminaryContactMessage(new contact::PreliminaryContactMessageHeader(receiverId, messageId, time, MessageFlagsFactory::createReceiptMessageFlags()), new contact::ReceiptMessageContent({ relatedMessage }, receiptType));
		}

		group::PreliminaryGroupMessage PreliminaryMessageFactory::createPreliminaryGroupTextMessage(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, QSet<openmittsu::protocol::ContactId> const& recipients, QString const& text) {
			return group::PreliminaryGroupMessage(new group::PreliminaryGroupMessageHeader(groupId, messageId, time, MessageFlagsFactory::createGroupTextMessageFlags()), new group::GroupTextMessageContent(groupId, text), recipients);
		}

		group::PreliminaryGroupMessage PreliminaryMessageFactory::createPreliminaryGroupImageMessage(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, QSet<openmittsu::protocol::ContactId> const& recipients, QByteArray const& imageData) {
			return group::PreliminaryGroupMessage(new group::PreliminaryGroupMessageHeader(groupId, messageId, time, MessageFlagsFactory::createGroupTextMessageFlags()), new group::GroupImageMessageContent(groupId, imageData), recipients);
		}

		group::PreliminaryGroupMessage PreliminaryMessageFactory::createPreliminaryGroupLocationMessage(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, QSet<openmittsu::protocol::ContactId> const& recipients, openmittsu::utility::Location const& location) {
			return group::PreliminaryGroupMessage(new group::PreliminaryGroupMessageHeader(groupId, messageId, time, MessageFlagsFactory::createGroupTextMessageFlags()), new group::GroupLocationMessageContent(groupId, location.getLatitude(), location.getLongitude(), location.getHeight(), location.getDescription()), recipients);
		}

		group::PreliminaryGroupMessage PreliminaryMessageFactory::createPreliminaryGroupSyncRequestMessage(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time) {
			return group::PreliminaryGroupMessage(new group::PreliminaryGroupMessageHeader(groupId, messageId, time, MessageFlagsFactory::createGroupControlMessageFlags()), new group::GroupSyncMessageContent(groupId), {groupId.getOwner()});
		}

		group::PreliminaryGroupMessage PreliminaryMessageFactory::createPreliminaryGroupCreationMessage(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, QSet<openmittsu::protocol::ContactId> const& recipients, QSet<openmittsu::protocol::ContactId> const& members) {
			return group::PreliminaryGroupMessage(new group::PreliminaryGroupMessageHeader(groupId, messageId, time, MessageFlagsFactory::createGroupControlMessageFlags()), new group::GroupCreationMessageContent(groupId, members), recipients);
		}

		group::PreliminaryGroupMessage PreliminaryMessageFactory::createPreliminaryGroupLeaveMessage(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, QSet<openmittsu::protocol::ContactId> const& recipients, openmittsu::protocol::ContactId const& ourContactId) {
			return group::PreliminaryGroupMessage(new group::PreliminaryGroupMessageHeader(groupId, messageId, time, MessageFlagsFactory::createGroupControlMessageFlags()), new group::GroupLeaveMessageContent(groupId, ourContactId), recipients);
		}

		group::PreliminaryGroupMessage PreliminaryMessageFactory::createPreliminaryGroupSetImageMessage(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, QSet<openmittsu::protocol::ContactId> const& recipients, QByteArray const& image) {
			return group::PreliminaryGroupMessage(new group::PreliminaryGroupMessageHeader(groupId, messageId, time, MessageFlagsFactory::createGroupControlMessageFlags()), new group::GroupSetPhotoMessageContent(groupId, image), recipients);
		}

		group::PreliminaryGroupMessage PreliminaryMessageFactory::createPreliminaryGroupSetTitleMessage(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, QSet<openmittsu::protocol::ContactId> const& recipients, QString const& title) {
			return group::PreliminaryGroupMessage(new group::PreliminaryGroupMessageHeader(groupId, messageId, time, MessageFlagsFactory::createGroupControlMessageFlags()), new group::GroupSetTitleMessageContent(groupId, title), recipients);
		}

	}
}
