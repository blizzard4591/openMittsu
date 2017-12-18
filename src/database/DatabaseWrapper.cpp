#include "src/database/DatabaseWrapper.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace database {

		DatabaseWrapper::DatabaseWrapper(std::shared_ptr<Database> const& database) : Database(), m_database(database), m_isConnected(false) {
			//
		}

		DatabaseWrapper::~DatabaseWrapper() {
			//
		}

		void DatabaseWrapper::setupConnection() {

		}

		openmittsu::protocol::GroupStatus DatabaseWrapper::getGroupStatus(openmittsu::protocol::GroupId const& group) const {
			openmittsu::protocol::GroupStatus returnVal;
			if (!QMetaObject::invokeMethod(m_database.get(), "getGroupStatus", Q_RETURN_ARG(openmittsu::protocol::GroupStatus, returnVal), Q_ARG(openmittsu::protocol::GroupId const&, group))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke getGroupStatus from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::ContactStatus DatabaseWrapper::getContactStatus(openmittsu::protocol::ContactId const & contact) const {
			return openmittsu::protocol::ContactStatus();
		}

		openmittsu::protocol::ContactId const & DatabaseWrapper::getSelfContact() const {
			// TODO: insert return statement here
		}

		bool DatabaseWrapper::hasContact(openmittsu::protocol::ContactId const & identity) const {
			return false;
		}

		bool DatabaseWrapper::hasGroup(openmittsu::protocol::GroupId const & group) const {
			return false;
		}

		bool DatabaseWrapper::isDeleteted(openmittsu::protocol::GroupId const & group) const {
			return false;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageText(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, QString const & message) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageImage(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, QByteArray const & image, QString const & caption) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageLocation(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, openmittsu::utility::Location const & location) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageReceiptReceived(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, openmittsu::protocol::MessageId const & referredMessageId) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageReceiptSeen(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, openmittsu::protocol::MessageId const & referredMessageId) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageReceiptAgree(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, openmittsu::protocol::MessageId const & referredMessageId) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageReceiptDisagree(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, openmittsu::protocol::MessageId const & referredMessageId) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageNotificationTypingStarted(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageNotificationTypingStopped(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupMessageText(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, QString const & message) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupMessageImage(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, QByteArray const & image, QString const & caption) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupMessageLocation(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, openmittsu::utility::Location const & location) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupCreation(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, QSet<openmittsu::protocol::ContactId> const & members, bool apply) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupSetImage(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, QByteArray const & image, bool apply) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupSetTitle(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, QString const & groupTitle, bool apply) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupSyncRequest(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupLeave(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, bool apply) {
			return openmittsu::protocol::MessageId();
		}

		void DatabaseWrapper::storeReceivedContactMessageText(openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived, QString const & message) {
		}

		void DatabaseWrapper::storeReceivedContactMessageImage(openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived, QByteArray const & image, QString const & caption) {
		}

		void DatabaseWrapper::storeReceivedContactMessageLocation(openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived, openmittsu::utility::Location const & location) {
		}

		void DatabaseWrapper::storeReceivedContactMessageReceiptReceived(openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageId const & referredMessageId) {
		}

		void DatabaseWrapper::storeReceivedContactMessageReceiptSeen(openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageId const & referredMessageId) {
		}

		void DatabaseWrapper::storeReceivedContactMessageReceiptAgree(openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageId const & referredMessageId) {
		}

		void DatabaseWrapper::storeReceivedContactMessageReceiptDisagree(openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageId const & referredMessageId) {
		}

		void DatabaseWrapper::storeReceivedContactTypingNotificationTyping(openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent) {
		}

		void DatabaseWrapper::storeReceivedContactTypingNotificationStopped(openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent) {
		}

		void DatabaseWrapper::storeReceivedGroupMessageText(openmittsu::protocol::GroupId const & group, openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived, QString const & message) {
		}

		void DatabaseWrapper::storeReceivedGroupMessageImage(openmittsu::protocol::GroupId const & group, openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived, QByteArray const & image, QString const & caption) {
		}

		void DatabaseWrapper::storeReceivedGroupMessageLocation(openmittsu::protocol::GroupId const & group, openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived, openmittsu::utility::Location const & location) {
		}

		void DatabaseWrapper::storeReceivedGroupCreation(openmittsu::protocol::GroupId const & group, openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived, QSet<openmittsu::protocol::ContactId> const & members) {
		}

		void DatabaseWrapper::storeReceivedGroupSetImage(openmittsu::protocol::GroupId const & group, openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived, QByteArray const & image) {
		}

		void DatabaseWrapper::storeReceivedGroupSetTitle(openmittsu::protocol::GroupId const & group, openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived, QString const & groupTitle) {
		}

		void DatabaseWrapper::storeReceivedGroupSyncRequest(openmittsu::protocol::GroupId const & group, openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived) {
		}

		void DatabaseWrapper::storeReceivedGroupLeave(openmittsu::protocol::GroupId const & group, openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived) {
		}

		void DatabaseWrapper::storeMessageSendFailed(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageId const & messageId) {
		}

		void DatabaseWrapper::storeMessageSendDone(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageId const & messageId) {
		}

		void DatabaseWrapper::storeMessageSendFailed(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageId const & messageId) {
		}

		void DatabaseWrapper::storeMessageSendDone(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageId const & messageId) {
		}

		void DatabaseWrapper::storeNewContact(openmittsu::protocol::ContactId const & contact, openmittsu::crypto::PublicKey const & publicKey) {
		}

		void DatabaseWrapper::storeNewGroup(openmittsu::protocol::GroupId const & groupId, QSet<openmittsu::protocol::ContactId> const & members, bool isAwaitingSync) {
		}

		void DatabaseWrapper::sendAllWaitingMessages(openmittsu::dataproviders::SentMessageAcceptor & messageAcceptor) {
		}

		openmittsu::dataproviders::BackedContact DatabaseWrapper::getBackedContact(openmittsu::protocol::ContactId const & contact, openmittsu::dataproviders::MessageCenter & messageCenter) {
			return openmittsu::dataproviders::BackedContact();
		}

		openmittsu::dataproviders::BackedGroup DatabaseWrapper::getBackedGroup(openmittsu::protocol::GroupId const & group, openmittsu::dataproviders::MessageCenter & messageCenter) {
			return openmittsu::dataproviders::BackedGroup();
		}

		QSet<openmittsu::protocol::ContactId> DatabaseWrapper::getGroupMembers(openmittsu::protocol::GroupId const & group, bool excludeSelfContact) const {
			return QSet<openmittsu::protocol::ContactId>();
		}

		void DatabaseWrapper::enableTimers() {
		}

		

	}
}
