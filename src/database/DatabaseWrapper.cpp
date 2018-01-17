#include "src/database/DatabaseWrapper.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace database {

		DatabaseWrapper::DatabaseWrapper(DatabasePointerAuthority const& databasePointerAuthority) : Database(), m_databasePointerAuthority(databasePointerAuthority), m_database() {
			OPENMITTSU_CONNECT_QUEUED(&m_databasePointerAuthority, newDatabaseAvailable(), this, onDatabasePointerAuthorityHasNewDatabase());
			m_database = m_databasePointerAuthority.getDatabaseWeak();
		}

		DatabaseWrapper::~DatabaseWrapper() {
			//
		}

		void DatabaseWrapper::onDatabasePointerAuthorityHasNewDatabase() {
			m_database = m_databasePointerAuthority.getDatabaseWeak();
		}

		openmittsu::protocol::GroupStatus DatabaseWrapper::getGroupStatus(openmittsu::protocol::GroupId const& group) const {
			openmittsu::protocol::GroupStatus returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "getGroupStatus", Q_RETURN_ARG(openmittsu::protocol::GroupStatus, returnVal), Q_ARG(openmittsu::protocol::GroupId const&, group))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke getGroupStatus from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::ContactStatus DatabaseWrapper::getContactStatus(openmittsu::protocol::ContactId const& contact) const {
			openmittsu::protocol::ContactStatus returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "getContactStatus", Q_RETURN_ARG(openmittsu::protocol::ContactStatus, returnVal), Q_ARG(openmittsu::protocol::ContactId const&, contact))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke getContactStatus from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::ContactId const& DatabaseWrapper::getSelfContact() const {
			openmittsu::protocol::ContactId returnVal(0);

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "getSelfContact", Q_RETURN_ARG(openmittsu::protocol::ContactId, returnVal))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke getSelfContact from Wrapper";
			}

			return returnVal;
		}

		bool DatabaseWrapper::hasContact(openmittsu::protocol::ContactId const& contact) const {
			bool returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "hasContact", Q_RETURN_ARG(bool, returnVal), Q_ARG(openmittsu::protocol::ContactId const&, contact))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke hasContact from Wrapper";
			}

			return returnVal;
		}

		bool DatabaseWrapper::hasGroup(openmittsu::protocol::GroupId const& group) const {
			bool returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "hasGroup", Q_RETURN_ARG(bool, returnVal), Q_ARG(openmittsu::protocol::GroupId const&, group))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke hasGroup from Wrapper";
			}

			return returnVal;
		}

		bool DatabaseWrapper::isDeleteted(openmittsu::protocol::GroupId const& group) const {
			bool returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "isDeleteted", Q_RETURN_ARG(bool, returnVal), Q_ARG(openmittsu::protocol::GroupId const&, group))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke isDeleteted from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageText(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& message) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageImage(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, QString const& caption) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageLocation(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::utility::Location const& location) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageReceiptReceived(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageReceiptSeen(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageReceiptAgree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageNotificationTypingStarted(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageNotificationTypingStopped(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& message) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, QString const& caption) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::utility::Location const& location) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QSet<openmittsu::protocol::ContactId> const& members, bool apply) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, bool apply) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& groupTitle, bool apply) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) {
			return openmittsu::protocol::MessageId();
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, bool apply) {
			return openmittsu::protocol::MessageId();
		}

		void DatabaseWrapper::storeReceivedContactMessageText(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) {
		}

		void DatabaseWrapper::storeReceivedContactMessageImage(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image, QString const& caption) {
		}

		void DatabaseWrapper::storeReceivedContactMessageLocation(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) {
		}

		void DatabaseWrapper::storeReceivedContactMessageReceiptReceived(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
		}

		void DatabaseWrapper::storeReceivedContactMessageReceiptSeen(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
		}

		void DatabaseWrapper::storeReceivedContactMessageReceiptAgree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
		}

		void DatabaseWrapper::storeReceivedContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
		}

		void DatabaseWrapper::storeReceivedContactTypingNotificationTyping(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
		}

		void DatabaseWrapper::storeReceivedContactTypingNotificationStopped(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
		}

		void DatabaseWrapper::storeReceivedGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) {
		}

		void DatabaseWrapper::storeReceivedGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image, QString const& caption) {
		}

		void DatabaseWrapper::storeReceivedGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) {
		}

		void DatabaseWrapper::storeReceivedGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QSet<openmittsu::protocol::ContactId> const& members) {
		}

		void DatabaseWrapper::storeReceivedGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) {
		}

		void DatabaseWrapper::storeReceivedGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& groupTitle) {
		}

		void DatabaseWrapper::storeReceivedGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) {
		}

		void DatabaseWrapper::storeReceivedGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) {
		}

		void DatabaseWrapper::storeMessageSendFailed(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) {
		}

		void DatabaseWrapper::storeMessageSendDone(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) {
		}

		void DatabaseWrapper::storeMessageSendFailed(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) {
		}

		void DatabaseWrapper::storeMessageSendDone(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) {
		}

		void DatabaseWrapper::storeNewContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey) {
		}

		void DatabaseWrapper::storeNewGroup(openmittsu::protocol::GroupId const& groupId, QSet<openmittsu::protocol::ContactId> const& members, bool isAwaitingSync) {
		}

		void DatabaseWrapper::sendAllWaitingMessages(openmittsu::dataproviders::SentMessageAcceptor & messageAcceptor) {
		}

		openmittsu::dataproviders::BackedContact DatabaseWrapper::getBackedContact(openmittsu::protocol::ContactId const& contact, openmittsu::dataproviders::MessageCenter & messageCenter) {
			return openmittsu::dataproviders::BackedContact();
		}

		openmittsu::dataproviders::BackedGroup DatabaseWrapper::getBackedGroup(openmittsu::protocol::GroupId const& group, openmittsu::dataproviders::MessageCenter & messageCenter) {
			return openmittsu::dataproviders::BackedGroup();
		}

		QSet<openmittsu::protocol::ContactId> DatabaseWrapper::getGroupMembers(openmittsu::protocol::GroupId const& group, bool excludeSelfContact) const {
			return QSet<openmittsu::protocol::ContactId>();
		}

		void DatabaseWrapper::enableTimers() {
		}

		

	}
}
