#ifndef OPENMITTSU_DATABASE_SENTMESSAGEACCEPTOR_H_
#define OPENMITTSU_DATABASE_SENTMESSAGEACCEPTOR_H_

#include <QByteArray>
#include <QObject>
#include <QSet>
#include <QString>

#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"

#include "src/utility/Location.h"

namespace openmittsu {
	namespace dataproviders {

		class SentMessageAcceptor {
		public:
			virtual ~SentMessageAcceptor() {}

			virtual void processSentContactMessageText(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QString const& message) = 0;
			virtual void processSentContactMessageImage(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QByteArray const& image, QString const& caption) = 0;
			virtual void processSentContactMessageLocation(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::utility::Location const& location) = 0;

			virtual void processSentContactMessageReceiptReceived(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) = 0;
			virtual void processSentContactMessageReceiptSeen(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) = 0;
			virtual void processSentContactMessageReceiptAgree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) = 0;
			virtual void processSentContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) = 0;

			virtual void processSentContactMessageTypingStarted(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) = 0;
			virtual void processSentContactMessageTypingStopped(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) = 0;

			virtual void processSentGroupMessageText(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QString const& message) = 0;
			virtual void processSentGroupMessageImage(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QByteArray const& image, QString const& caption) = 0;
			virtual void processSentGroupMessageLocation(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::utility::Location const& location) = 0;

			virtual void processSentGroupCreation(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QSet<openmittsu::protocol::ContactId> const& members) = 0;
			virtual void processSentGroupSetImage(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QByteArray const& image) = 0;
			virtual void processSentGroupSetTitle(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QString const& groupTitle) = 0;
			virtual void processSentGroupSyncRequest(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) = 0;
			virtual void processSentGroupLeave(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::ContactId const& ourContactId) = 0;
		};

	}
}

#endif // OPENMITTSU_DATABASE_SENTMESSAGEACCEPTOR_H_
