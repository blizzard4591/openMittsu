#ifndef OPENMITTSU_MESSAGES_PRELIMINARYMESSAGEFACTORY_H_
#define OPENMITTSU_MESSAGES_PRELIMINARYMESSAGEFACTORY_H_

#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/MessageId.h"
#include "src/messages/contact/PreliminaryContactMessage.h"
#include "src/messages/group/PreliminaryGroupMessage.h"
#include "src/messages/contact/ReceiptMessageContent.h"
#include "src/utility/Location.h"

#include <QByteArray>
#include <QString>
#include <QSet>

namespace openmittsu {
	namespace messages {

		class PreliminaryMessageFactory {
		public:
			// Contact
			static contact::PreliminaryContactMessage createPreliminaryContactTextMessage(openmittsu::protocol::ContactId const& receiverId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, QString const& text);
			static contact::PreliminaryContactMessage createPreliminaryContactImageMessage(openmittsu::protocol::ContactId const& receiverId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, QByteArray const& imageData);
			static contact::PreliminaryContactMessage createPreliminaryContactLocationMessage(openmittsu::protocol::ContactId const& receiverId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, openmittsu::utility::Location const& location);

			static contact::PreliminaryContactMessage createPreliminaryContactUserTypingStartedMessage(openmittsu::protocol::ContactId const& receiverId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time);
			static contact::PreliminaryContactMessage createPreliminaryContactUserTypingStoppedMessage(openmittsu::protocol::ContactId const& receiverId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time);

			static contact::PreliminaryContactMessage createPreliminaryContactMessageReceipt(openmittsu::protocol::ContactId const& receiverId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, openmittsu::protocol::MessageId const& relatedMessage, contact::ReceiptMessageContent::ReceiptType const& receiptType);

			// Groups
			static group::PreliminaryGroupMessage createPreliminaryGroupTextMessage(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, QSet<openmittsu::protocol::ContactId> const& recipients, QString const& text);
			static group::PreliminaryGroupMessage createPreliminaryGroupImageMessage(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, QSet<openmittsu::protocol::ContactId> const& recipients, QByteArray const& imageData);
			static group::PreliminaryGroupMessage createPreliminaryGroupLocationMessage(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, QSet<openmittsu::protocol::ContactId> const& recipients, openmittsu::utility::Location const& location);

			static group::PreliminaryGroupMessage createPreliminaryGroupSyncRequestMessage(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time);
			static group::PreliminaryGroupMessage createPreliminaryGroupCreationMessage(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, QSet<openmittsu::protocol::ContactId> const& recipients, QSet<openmittsu::protocol::ContactId> const& members);
			static group::PreliminaryGroupMessage createPreliminaryGroupLeaveMessage(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, QSet<openmittsu::protocol::ContactId> const& recipients, openmittsu::protocol::ContactId const& ourContactId);
			static group::PreliminaryGroupMessage createPreliminaryGroupSetImageMessage(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, QSet<openmittsu::protocol::ContactId> const& recipients, QByteArray const& image);
			static group::PreliminaryGroupMessage createPreliminaryGroupSetTitleMessage(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, QSet<openmittsu::protocol::ContactId> const& recipients, QString const& title);
		private:
			// Disable the constructors
			PreliminaryMessageFactory();
			PreliminaryMessageFactory(PreliminaryMessageFactory const& other);
			virtual ~PreliminaryMessageFactory();
		};

	}
}

#endif // OPENMITTSU_MESSAGES_PRELIMINARYMESSAGEFACTORY_H_
