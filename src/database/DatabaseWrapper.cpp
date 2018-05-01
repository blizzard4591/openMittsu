#include "src/database/DatabaseWrapper.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"
#include "src/utility/Wrapping.h"

#include "src/database/internal/DatabaseContactMessageCursor.h"
#include "src/database/internal/DatabaseGroupMessageCursor.h"

namespace openmittsu {
	namespace database {

		DatabaseWrapper::DatabaseWrapper(DatabasePointerAuthority const& databasePointerAuthority) : Database(), m_databasePointerAuthority(databasePointerAuthority), m_database() {
			OPENMITTSU_CONNECT_QUEUED(&m_databasePointerAuthority, newDatabaseAvailable(), this, onDatabasePointerAuthorityHasNewDatabase());
			onDatabasePointerAuthorityHasNewDatabase();
		}

		DatabaseWrapper::DatabaseWrapper(DatabaseWrapper const& other) : Database(), m_databasePointerAuthority(other.m_databasePointerAuthority), m_database() {
			OPENMITTSU_CONNECT_QUEUED(&m_databasePointerAuthority, newDatabaseAvailable(), this, onDatabasePointerAuthorityHasNewDatabase());
			onDatabasePointerAuthorityHasNewDatabase();
		}

		DatabaseWrapper::~DatabaseWrapper() {
			//
		}

		void DatabaseWrapper::onDatabasePointerAuthorityHasNewDatabase() {
			m_database = m_databasePointerAuthority.getDatabaseWeak();

			auto ptr = m_database.lock();
			if (ptr) {
				OPENMITTSU_CONNECT_QUEUED(ptr.get(), contactChanged(openmittsu::protocol::ContactId const&), this, onDatabaseContactChanged(openmittsu::protocol::ContactId const&));
				OPENMITTSU_CONNECT_QUEUED(ptr.get(), groupChanged(openmittsu::protocol::GroupId const&), this, onDatabaseGroupChanged(openmittsu::protocol::GroupId const&));
				OPENMITTSU_CONNECT_QUEUED(ptr.get(), contactHasNewMessage(openmittsu::protocol::ContactId const&, QString const&), this, onDatabaseContactHasNewMessage(openmittsu::protocol::ContactId const&, QString const&));
				OPENMITTSU_CONNECT_QUEUED(ptr.get(), groupHasNewMessage(openmittsu::protocol::GroupId const&, QString const&), this, onDatabaseGroupHasNewMessage(openmittsu::protocol::GroupId const&, QString const&));
				OPENMITTSU_CONNECT_QUEUED(ptr.get(), receivedNewContactMessage(openmittsu::protocol::ContactId const&), this, onDatabaseReceivedNewContactMessage(openmittsu::protocol::ContactId const&));
				OPENMITTSU_CONNECT_QUEUED(ptr.get(), receivedNewGroupMessage(openmittsu::protocol::GroupId const&), this, onDatabaseReceivedNewGroupMessage(openmittsu::protocol::GroupId const&));
				OPENMITTSU_CONNECT_QUEUED(ptr.get(), messageChanged(QString const&), this, onDatabaseMessageChanged(QString const&));
				OPENMITTSU_CONNECT_QUEUED(ptr.get(), haveQueuedMessages(), this, onDatabaseHaveQueuedMessages());
				OPENMITTSU_CONNECT_QUEUED(ptr.get(), contactStartedTyping(openmittsu::protocol::ContactId const&), this, onDatabaseContactStartedTyping(openmittsu::protocol::ContactId const&));
				OPENMITTSU_CONNECT_QUEUED(ptr.get(), contactStoppedTyping(openmittsu::protocol::ContactId const&), this, onDatabaseContactStoppedTyping(openmittsu::protocol::ContactId const&));
			}
		}

		void DatabaseWrapper::onDatabaseContactChanged(openmittsu::protocol::ContactId const& identity) {
			emit contactChanged(identity);
		}
		
		void DatabaseWrapper::onDatabaseGroupChanged(openmittsu::protocol::GroupId const& changedGroupId) {
			emit groupChanged(changedGroupId);
		}
		
		void DatabaseWrapper::onDatabaseContactHasNewMessage(openmittsu::protocol::ContactId const& identity, QString const& messageUuid) {
			emit contactHasNewMessage(identity, messageUuid);
		}
		
		void DatabaseWrapper::onDatabaseGroupHasNewMessage(openmittsu::protocol::GroupId const& group, QString const& messageUuid) {
			emit groupHasNewMessage(group, messageUuid);
		}
		
		void DatabaseWrapper::onDatabaseReceivedNewContactMessage(openmittsu::protocol::ContactId const& identity) {
			emit receivedNewContactMessage(identity);
		}
		
		void DatabaseWrapper::onDatabaseReceivedNewGroupMessage(openmittsu::protocol::GroupId const& group) {
			emit receivedNewGroupMessage(group);
		}
		
		void DatabaseWrapper::onDatabaseMessageChanged(QString const& uuid) {
			emit messageChanged(uuid);
		}
		
		void DatabaseWrapper::onDatabaseHaveQueuedMessages() {
			emit haveQueuedMessages();
		}
		
		void DatabaseWrapper::onDatabaseContactStartedTyping(openmittsu::protocol::ContactId const& identity) {
			emit contactStartedTyping(identity);
		}

		void DatabaseWrapper::onDatabaseContactStoppedTyping(openmittsu::protocol::ContactId const& identity) {
			emit contactStoppedTyping(identity);
		}

		openmittsu::protocol::GroupStatus DatabaseWrapper::getGroupStatus(openmittsu::protocol::GroupId const& group) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getGroupStatus, openmittsu::protocol::GroupStatus, Q_ARG(openmittsu::protocol::GroupId const&, group));
		}
		openmittsu::protocol::ContactStatus DatabaseWrapper::getContactStatus(openmittsu::protocol::ContactId const& contact) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getContactStatus, openmittsu::protocol::ContactStatus, Q_ARG(openmittsu::protocol::ContactId const&, contact));
		}

		openmittsu::protocol::ContactId DatabaseWrapper::getSelfContact() const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getSelfContact, openmittsu::protocol::ContactId);
		}

		bool DatabaseWrapper::hasContact(openmittsu::protocol::ContactId const& contact) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(hasContact, bool, Q_ARG(openmittsu::protocol::ContactId const&, contact));
		}

		bool DatabaseWrapper::hasGroup(openmittsu::protocol::GroupId const& group) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(hasGroup, bool, Q_ARG(openmittsu::protocol::GroupId const&, group));
		}

		bool DatabaseWrapper::isDeleteted(openmittsu::protocol::GroupId const& group) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(isDeleteted, bool, Q_ARG(openmittsu::protocol::GroupId const&, group));
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageText(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& message) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(storeSentContactMessageText, openmittsu::protocol::MessageId, Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(QString const&, message));
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageImage(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, QString const& caption) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(storeSentContactMessageImage, openmittsu::protocol::MessageId, Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(QByteArray const&, image), Q_ARG(QString const&, caption));
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageLocation(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::utility::Location const& location) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(storeSentContactMessageLocation, openmittsu::protocol::MessageId, Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(openmittsu::utility::Location const&, location));
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageReceiptReceived(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(storeSentContactMessageReceiptReceived, openmittsu::protocol::MessageId, Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(openmittsu::protocol::MessageId const&, referredMessageId));
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageReceiptSeen(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(storeSentContactMessageReceiptSeen, openmittsu::protocol::MessageId, Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(openmittsu::protocol::MessageId const&, referredMessageId));
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageReceiptAgree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(storeSentContactMessageReceiptAgree, openmittsu::protocol::MessageId, Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(openmittsu::protocol::MessageId const&, referredMessageId));
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(storeSentContactMessageReceiptDisagree, openmittsu::protocol::MessageId, Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(openmittsu::protocol::MessageId const&, referredMessageId));
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageNotificationTypingStarted(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(storeSentContactMessageNotificationTypingStarted, openmittsu::protocol::MessageId, Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued));
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageNotificationTypingStopped(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(storeSentContactMessageNotificationTypingStopped, openmittsu::protocol::MessageId, Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued));
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& message) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(storeSentGroupMessageText, openmittsu::protocol::MessageId, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(QString const&, message));
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, QString const& caption) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(storeSentGroupMessageImage, openmittsu::protocol::MessageId, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(QByteArray const&, image), Q_ARG(QString const&, caption));
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::utility::Location const& location) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(storeSentGroupMessageLocation, openmittsu::protocol::MessageId, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(openmittsu::utility::Location const&, location));
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QSet<openmittsu::protocol::ContactId> const& members, bool apply) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(storeSentGroupCreation, openmittsu::protocol::MessageId, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(QSet<openmittsu::protocol::ContactId> const&, members), Q_ARG(bool, apply));
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, bool apply) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(storeSentGroupSetImage, openmittsu::protocol::MessageId, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(QByteArray const&, image), Q_ARG(bool, apply));
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& groupTitle, bool apply) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(storeSentGroupSetTitle, openmittsu::protocol::MessageId, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(QString const&, groupTitle), Q_ARG(bool, apply));
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(storeSentGroupSyncRequest, openmittsu::protocol::MessageId, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued));
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, bool apply) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(storeSentGroupLeave, openmittsu::protocol::MessageId, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(bool, apply));
		}

		void DatabaseWrapper::storeReceivedContactMessageText(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeReceivedContactMessageText, Q_ARG(openmittsu::protocol::ContactId const&, sender), Q_ARG(openmittsu::protocol::MessageId const&, messageId), Q_ARG(openmittsu::protocol::MessageTime const&, timeSent), Q_ARG(openmittsu::protocol::MessageTime const&, timeReceived), Q_ARG(QString const&, message));
		}

		void DatabaseWrapper::storeReceivedContactMessageImage(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image, QString const& caption) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeReceivedContactMessageImage, Q_ARG(openmittsu::protocol::ContactId const&, sender), Q_ARG(openmittsu::protocol::MessageId const&, messageId), Q_ARG(openmittsu::protocol::MessageTime const&, timeSent), Q_ARG(openmittsu::protocol::MessageTime const&, timeReceived), Q_ARG(QByteArray const&, image), Q_ARG(QString const&, caption));
		}

		void DatabaseWrapper::storeReceivedContactMessageLocation(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeReceivedContactMessageLocation, Q_ARG(openmittsu::protocol::ContactId const&, sender), Q_ARG(openmittsu::protocol::MessageId const&, messageId), Q_ARG(openmittsu::protocol::MessageTime const&, timeSent), Q_ARG(openmittsu::protocol::MessageTime const&, timeReceived), Q_ARG(openmittsu::utility::Location const&, location));
		}

		void DatabaseWrapper::storeReceivedContactMessageReceiptReceived(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeReceivedContactMessageReceiptReceived, Q_ARG(openmittsu::protocol::ContactId const&, sender), Q_ARG(openmittsu::protocol::MessageId const&, messageId), Q_ARG(openmittsu::protocol::MessageTime const&, timeSent), Q_ARG(openmittsu::protocol::MessageId const&, referredMessageId));
		}

		void DatabaseWrapper::storeReceivedContactMessageReceiptSeen(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeReceivedContactMessageReceiptSeen, Q_ARG(openmittsu::protocol::ContactId const&, sender), Q_ARG(openmittsu::protocol::MessageId const&, messageId), Q_ARG(openmittsu::protocol::MessageTime const&, timeSent), Q_ARG(openmittsu::protocol::MessageId const&, referredMessageId));
		}

		void DatabaseWrapper::storeReceivedContactMessageReceiptAgree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeReceivedContactMessageReceiptAgree, Q_ARG(openmittsu::protocol::ContactId const&, sender), Q_ARG(openmittsu::protocol::MessageId const&, messageId), Q_ARG(openmittsu::protocol::MessageTime const&, timeSent), Q_ARG(openmittsu::protocol::MessageId const&, referredMessageId));
		}

		void DatabaseWrapper::storeReceivedContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeReceivedContactMessageReceiptDisagree, Q_ARG(openmittsu::protocol::ContactId const&, sender), Q_ARG(openmittsu::protocol::MessageId const&, messageId), Q_ARG(openmittsu::protocol::MessageTime const&, timeSent), Q_ARG(openmittsu::protocol::MessageId const&, referredMessageId));
		}

		void DatabaseWrapper::storeReceivedContactTypingNotificationTyping(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeReceivedContactTypingNotificationTyping, Q_ARG(openmittsu::protocol::ContactId const&, sender), Q_ARG(openmittsu::protocol::MessageId const&, messageId), Q_ARG(openmittsu::protocol::MessageTime const&, timeSent));
		}

		void DatabaseWrapper::storeReceivedContactTypingNotificationStopped(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeReceivedContactTypingNotificationStopped, Q_ARG(openmittsu::protocol::ContactId const&, sender), Q_ARG(openmittsu::protocol::MessageId const&, messageId), Q_ARG(openmittsu::protocol::MessageTime const&, timeSent));
		}

		void DatabaseWrapper::storeReceivedGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeReceivedGroupMessageText, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::ContactId const&, sender), Q_ARG(openmittsu::protocol::MessageId const&, messageId), Q_ARG(openmittsu::protocol::MessageTime const&, timeSent), Q_ARG(openmittsu::protocol::MessageTime const&, timeReceived), Q_ARG(QString const&, message));
		}

		void DatabaseWrapper::storeReceivedGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image, QString const& caption) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeReceivedGroupMessageImage, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::ContactId const&, sender), Q_ARG(openmittsu::protocol::MessageId const&, messageId), Q_ARG(openmittsu::protocol::MessageTime const&, timeSent), Q_ARG(openmittsu::protocol::MessageTime const&, timeReceived), Q_ARG(QByteArray const&, image), Q_ARG(QString const&, caption));
		}

		void DatabaseWrapper::storeReceivedGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeReceivedGroupMessageLocation, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::ContactId const&, sender), Q_ARG(openmittsu::protocol::MessageId const&, messageId), Q_ARG(openmittsu::protocol::MessageTime const&, timeSent), Q_ARG(openmittsu::protocol::MessageTime const&, timeReceived), Q_ARG(openmittsu::utility::Location const&, location));
		}

		void DatabaseWrapper::storeReceivedGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QSet<openmittsu::protocol::ContactId> const& members) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeReceivedGroupCreation, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::ContactId const&, sender), Q_ARG(openmittsu::protocol::MessageId const&, messageId), Q_ARG(openmittsu::protocol::MessageTime const&, timeSent), Q_ARG(openmittsu::protocol::MessageTime const&, timeReceived), Q_ARG(QSet<openmittsu::protocol::ContactId> const&, members));
		}

		void DatabaseWrapper::storeReceivedGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeReceivedGroupSetImage, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::ContactId const&, sender), Q_ARG(openmittsu::protocol::MessageId const&, messageId), Q_ARG(openmittsu::protocol::MessageTime const&, timeSent), Q_ARG(openmittsu::protocol::MessageTime const&, timeReceived), Q_ARG(QByteArray const&, image));
		}

		void DatabaseWrapper::storeReceivedGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& groupTitle) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeReceivedGroupSetTitle, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::ContactId const&, sender), Q_ARG(openmittsu::protocol::MessageId const&, messageId), Q_ARG(openmittsu::protocol::MessageTime const&, timeSent), Q_ARG(openmittsu::protocol::MessageTime const&, timeReceived), Q_ARG(QString const&, groupTitle));
		}

		void DatabaseWrapper::storeReceivedGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeReceivedGroupSyncRequest, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::ContactId const&, sender), Q_ARG(openmittsu::protocol::MessageId const&, messageId), Q_ARG(openmittsu::protocol::MessageTime const&, timeSent), Q_ARG(openmittsu::protocol::MessageTime const&, timeReceived));
		}

		void DatabaseWrapper::storeReceivedGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeReceivedGroupLeave, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::ContactId const&, sender), Q_ARG(openmittsu::protocol::MessageId const&, messageId), Q_ARG(openmittsu::protocol::MessageTime const&, timeSent), Q_ARG(openmittsu::protocol::MessageTime const&, timeReceived));
		}

		void DatabaseWrapper::storeMessageSendFailed(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeMessageSendFailed, Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageId const&, messageId));
		}

		void DatabaseWrapper::storeMessageSendDone(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeMessageSendDone, Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageId const&, messageId));
		}

		void DatabaseWrapper::storeMessageSendFailed(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeMessageSendFailed, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::MessageId const&, messageId));
		}

		void DatabaseWrapper::storeMessageSendDone(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeMessageSendDone, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::MessageId const&, messageId));
		}

		void DatabaseWrapper::storeNewContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeNewContact, Q_ARG(openmittsu::protocol::ContactId const&, contact), Q_ARG(openmittsu::crypto::PublicKey const&, publicKey));
		}

		void DatabaseWrapper::storeNewGroup(openmittsu::protocol::GroupId const& groupId, QSet<openmittsu::protocol::ContactId> const& members, bool isAwaitingSync) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(storeNewGroup, Q_ARG(openmittsu::protocol::GroupId const&, groupId), Q_ARG(QSet<openmittsu::protocol::ContactId> const&, members), Q_ARG(bool, isAwaitingSync));
		}

		void DatabaseWrapper::sendAllWaitingMessages(openmittsu::dataproviders::SentMessageAcceptor & messageAcceptor) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(sendAllWaitingMessages, Q_ARG(openmittsu::dataproviders::SentMessageAcceptor&, messageAcceptor));
		}

		std::unique_ptr<openmittsu::dataproviders::BackedContact> DatabaseWrapper::getBackedContact(openmittsu::protocol::ContactId const& contact, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getBackedContact, std::unique_ptr<openmittsu::dataproviders::BackedContact>, Q_ARG(openmittsu::protocol::ContactId const&, contact), Q_ARG(openmittsu::dataproviders::MessageCenterWrapper const&, messageCenter));
		}

		std::unique_ptr<openmittsu::dataproviders::BackedGroup> DatabaseWrapper::getBackedGroup(openmittsu::protocol::GroupId const& group, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getBackedGroup, std::unique_ptr<openmittsu::dataproviders::BackedGroup>, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::dataproviders::MessageCenterWrapper const&, messageCenter));
		}

		QSet<openmittsu::protocol::ContactId> DatabaseWrapper::getGroupMembers(openmittsu::protocol::GroupId const& group, bool excludeSelfContact) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getGroupMembers, QSet<openmittsu::protocol::ContactId>, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(bool, excludeSelfContact));
		}

		void DatabaseWrapper::enableTimers() {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(enableTimers);
		}

		QString DatabaseWrapper::getFirstName(openmittsu::protocol::ContactId const& contact) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getFirstName, QString, Q_ARG(openmittsu::protocol::ContactId const&, contact));
		}
		
		QString DatabaseWrapper::getLastName(openmittsu::protocol::ContactId const& contact) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getLastName, QString, Q_ARG(openmittsu::protocol::ContactId const&, contact));
		}
		
		QString DatabaseWrapper::getNickName(openmittsu::protocol::ContactId const& contact) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getNickName, QString, Q_ARG(openmittsu::protocol::ContactId const&, contact));
		}
		
		openmittsu::protocol::AccountStatus DatabaseWrapper::getAccountStatus(openmittsu::protocol::ContactId const& contact) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getAccountStatus, openmittsu::protocol::AccountStatus, Q_ARG(openmittsu::protocol::ContactId const&, contact));
		}
		
		openmittsu::protocol::ContactIdVerificationStatus DatabaseWrapper::getVerificationStatus(openmittsu::protocol::ContactId const& contact) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getVerificationStatus, openmittsu::protocol::ContactIdVerificationStatus, Q_ARG(openmittsu::protocol::ContactId const&, contact));
		}
		
		openmittsu::protocol::FeatureLevel DatabaseWrapper::getFeatureLevel(openmittsu::protocol::ContactId const& contact) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getFeatureLevel, openmittsu::protocol::FeatureLevel, Q_ARG(openmittsu::protocol::ContactId const&, contact));
		}
		
		int DatabaseWrapper::getColor(openmittsu::protocol::ContactId const& contact) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getColor, int, Q_ARG(openmittsu::protocol::ContactId const&, contact));
		}
		
		int DatabaseWrapper::getContactCount() const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getContactCount, int);
		}
		
		int DatabaseWrapper::getContactMessageCount(openmittsu::protocol::ContactId const& contact) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getContactMessageCount, int, Q_ARG(openmittsu::protocol::ContactId const&, contact));
		}

		void DatabaseWrapper::setFirstName(openmittsu::protocol::ContactId const& contact, QString const& firstName) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(setFirstName, Q_ARG(openmittsu::protocol::ContactId const&, contact), Q_ARG(QString const&, firstName));
		}
		
		void DatabaseWrapper::setLastName(openmittsu::protocol::ContactId const& contact, QString const& lastName) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(setLastName, Q_ARG(openmittsu::protocol::ContactId const&, contact), Q_ARG(QString const&, lastName));
		}
		
		void DatabaseWrapper::setNickName(openmittsu::protocol::ContactId const& contact, QString const& nickname) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(setNickName, Q_ARG(openmittsu::protocol::ContactId const&, contact), Q_ARG(QString const&, nickname));
		}
		
		void DatabaseWrapper::setAccountStatus(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::AccountStatus const& status) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(setAccountStatus, Q_ARG(openmittsu::protocol::ContactId const&, contact), Q_ARG(openmittsu::protocol::AccountStatus const&, status));
		}
		
		void DatabaseWrapper::setVerificationStatus(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::ContactIdVerificationStatus const& verificationStatus) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(setVerificationStatus, Q_ARG(openmittsu::protocol::ContactId const&, contact), Q_ARG(openmittsu::protocol::ContactIdVerificationStatus const&, verificationStatus));
		}
		
		void DatabaseWrapper::setFeatureLevel(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::FeatureLevel const& featureLevel) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(setFeatureLevel, Q_ARG(openmittsu::protocol::ContactId const&, contact), Q_ARG(openmittsu::protocol::FeatureLevel const&, featureLevel));
		}
		
		void DatabaseWrapper::setColor(openmittsu::protocol::ContactId const& contact, int color) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(setColor, Q_ARG(openmittsu::protocol::ContactId const&, contact), Q_ARG(int, color));
		}

		QString DatabaseWrapper::getGroupTitle(openmittsu::protocol::GroupId const& group) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getGroupTitle, QString, Q_ARG(openmittsu::protocol::GroupId const&, group));
		}
		
		QString DatabaseWrapper::getGroupDescription(openmittsu::protocol::GroupId const& group) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getGroupDescription, QString, Q_ARG(openmittsu::protocol::GroupId const&, group));
		}
		
		bool DatabaseWrapper::getGroupHasImage(openmittsu::protocol::GroupId const& group) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getGroupHasImage, bool, Q_ARG(openmittsu::protocol::GroupId const&, group));
		}
		
		openmittsu::database::MediaFileItem DatabaseWrapper::getGroupImage(openmittsu::protocol::GroupId const& group) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getGroupImage, openmittsu::database::MediaFileItem, Q_ARG(openmittsu::protocol::GroupId const&, group));
		}
		
		bool DatabaseWrapper::getGroupIsAwaitingSync(openmittsu::protocol::GroupId const& group) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getGroupIsAwaitingSync, bool, Q_ARG(openmittsu::protocol::GroupId const&, group));
		}
		
		int DatabaseWrapper::getGroupCount() const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getGroupCount, int);
		}
		
		int DatabaseWrapper::getGroupMessageCount(openmittsu::protocol::GroupId const& group) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getGroupMessageCount, int, Q_ARG(openmittsu::protocol::GroupId const&, group));
		}

		void DatabaseWrapper::setGroupTitle(openmittsu::protocol::GroupId const& group, QString const& newTitle) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(setGroupTitle, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(QString const&, newTitle));
		}
		
		void DatabaseWrapper::setGroupImage(openmittsu::protocol::GroupId const& group, QByteArray const& newImage) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(setGroupImage, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(QByteArray const&, newImage));
		}
		
		void DatabaseWrapper::setGroupMembers(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& newMembers) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(setGroupMembers, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(QSet<openmittsu::protocol::ContactId> const&, newMembers));
		}

		std::shared_ptr<openmittsu::dataproviders::messages::ContactMessageCursor> DatabaseWrapper::getContactMessageCursor(openmittsu::protocol::ContactId const& contact) {
			return std::make_shared<openmittsu::database::internal::DatabaseContactMessageCursor>(m_database, group);
		}
		
		std::shared_ptr <openmittsu::dataproviders::messages::GroupMessageCursor> DatabaseWrapper::getGroupMessageCursor(openmittsu::protocol::GroupId const& group) {
			return std::make_shared<openmittsu::database::internal::DatabaseGroupMessageCursor>(m_database, group);
		}

		DatabaseSeekResult DatabaseWrapper::seekNextMessage(openmittsu::protocol::ContactId const& identity, QString const& uuid, SortOrder sortOrder, SortByMode sortByMode) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(seekNextMessage, DatabaseSeekResult, Q_ARG(openmittsu::protocol::ContactId const&, identity), Q_ARG(QString const&, uuid), Q_ARG(SortOrder, sortOrder), Q_ARG(SortByMode, sortByMode));
		}

		DatabaseSeekResult DatabaseWrapper::seekFirstOrLastMessage(openmittsu::protocol::ContactId const& identity, bool first, SortByMode sortByMode) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(seekFirstOrLastMessage, DatabaseSeekResult, Q_ARG(openmittsu::protocol::ContactId const&, identity), Q_ARG(bool, first), Q_ARG(SortByMode, sortByMode));
		}

		DatabaseSeekResult DatabaseWrapper::seekNextMessage(openmittsu::protocol::GroupId const& group, QString const& uuid, SortOrder sortOrder, SortByMode sortByMode) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(seekNextMessage, DatabaseSeekResult, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(QString const&, uuid), Q_ARG(SortOrder, sortOrder), Q_ARG(SortByMode, sortByMode));
		}

		DatabaseSeekResult DatabaseWrapper::seekFirstOrLastMessage(openmittsu::protocol::GroupId const& group, bool first, SortByMode sortByMode) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(seekFirstOrLastMessage, DatabaseSeekResult, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(bool, first), Q_ARG(SortByMode, sortByMode));
		}
	}
}
