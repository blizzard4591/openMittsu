#include "src/database/DatabaseWrapper.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)
#define OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(funcName, ...) do { \
auto ptr = m_database.lock(); \
if (!ptr) { \
	LOGGER()->error("Tried aquiring database pointer in wrapper for {} and failed!", STRINGIFY(funcName)); \
	throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for " << STRINGIFY(funcName) << " and failed!"; \
} \
 \
if (!QMetaObject::invokeMethod(ptr.get(), STRINGIFY(funcName), __VA_ARGS__)) { \
	throw openmittsu::exceptions::InternalErrorException() << "Could not invoke " << STRINGIFY(funcName) << " from Wrapper"; \
}\
} while (false)

#define OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(funcName, returnType, ...) do { \
returnType returnVal; \
 \
auto ptr = m_database.lock(); \
if (!ptr) { \
	LOGGER()->error("Tried aquiring database pointer in wrapper for {} and failed!", STRINGIFY(funcName)); \
	throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for " << STRINGIFY(funcName) << " and failed!"; \
} \
 \
if (!QMetaObject::invokeMethod(ptr.get(), STRINGIFY(funcName), Q_RETURN_ARG(returnType, returnVal), __VA_ARGS__)) { \
	throw openmittsu::exceptions::InternalErrorException() << "Could not invoke " << STRINGIFY(funcName) << " from Wrapper"; \
} \
 \
return returnVal; \
} while (false)


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
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getGroupStatus, openmittsu::protocol::GroupStatus, Q_ARG(openmittsu::protocol::GroupId const&, group));
		}

		openmittsu::protocol::ContactStatus DatabaseWrapper::getContactStatus(openmittsu::protocol::ContactId const& contact) const {
			openmittsu::protocol::ContactStatus returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for getContactStatus and failed!";
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
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for getSelfContact and failed!";
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
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for hasContact and failed!";
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
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for hasGroup and failed!";
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
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for isDeleted and failed!";
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "isDeleteted", Q_RETURN_ARG(bool, returnVal), Q_ARG(openmittsu::protocol::GroupId const&, group))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke isDeleteted from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageText(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& message) {
			openmittsu::protocol::MessageId returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for storeSentContactMessageText and failed!";
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "storeSentContactMessageText", Q_RETURN_ARG(openmittsu::protocol::MessageId, returnVal), Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(QString const&, message))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke storeSentContactMessageText from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageImage(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, QString const& caption) {
			openmittsu::protocol::MessageId returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for storeSentContactMessageImage and failed!";
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "storeSentContactMessageImage", Q_RETURN_ARG(openmittsu::protocol::MessageId, returnVal), Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(QByteArray const&, image), Q_ARG(QString const&, caption))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke storeSentContactMessageImage from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageLocation(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::utility::Location const& location) {
			openmittsu::protocol::MessageId returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for storeSentContactMessageLocation and failed!";
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "storeSentContactMessageLocation", Q_RETURN_ARG(openmittsu::protocol::MessageId, returnVal), Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(openmittsu::utility::Location const&, location))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke storeSentContactMessageLocation from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageReceiptReceived(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) {
			openmittsu::protocol::MessageId returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for storeSentContactMessageReceiptReceived and failed!";
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "storeSentContactMessageReceiptReceived", Q_RETURN_ARG(openmittsu::protocol::MessageId, returnVal), Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(openmittsu::protocol::MessageId const&, referredMessageId))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke storeSentContactMessageReceiptReceived from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageReceiptSeen(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) {
			openmittsu::protocol::MessageId returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for storeSentContactMessageReceiptSeen and failed!";
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "storeSentContactMessageReceiptSeen", Q_RETURN_ARG(openmittsu::protocol::MessageId, returnVal), Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(openmittsu::protocol::MessageId const&, referredMessageId))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke storeSentContactMessageReceiptSeen from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageReceiptAgree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) {
			openmittsu::protocol::MessageId returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for storeSentContactMessageReceiptAgree and failed!";
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "storeSentContactMessageReceiptAgree", Q_RETURN_ARG(openmittsu::protocol::MessageId, returnVal), Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(openmittsu::protocol::MessageId const&, referredMessageId))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke storeSentContactMessageReceiptAgree from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) {
			openmittsu::protocol::MessageId returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for storeSentContactMessageReceiptDisagree and failed!";
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "storeSentContactMessageReceiptDisagree", Q_RETURN_ARG(openmittsu::protocol::MessageId, returnVal), Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(openmittsu::protocol::MessageId const&, referredMessageId))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke storeSentContactMessageReceiptDisagree from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageNotificationTypingStarted(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) {
			openmittsu::protocol::MessageId returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for storeSentContactMessageNotificationTypingStarted and failed!";
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "storeSentContactMessageNotificationTypingStarted", Q_RETURN_ARG(openmittsu::protocol::MessageId, returnVal), Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke storeSentContactMessageNotificationTypingStarted from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentContactMessageNotificationTypingStopped(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) {
			openmittsu::protocol::MessageId returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for storeSentContactMessageNotificationTypingStopped and failed!";
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "storeSentContactMessageNotificationTypingStopped", Q_RETURN_ARG(openmittsu::protocol::MessageId, returnVal), Q_ARG(openmittsu::protocol::ContactId const&, receiver), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke storeSentContactMessageNotificationTypingStopped from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& message) {
			openmittsu::protocol::MessageId returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for storeSentGroupMessageText and failed!";
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "storeSentGroupMessageText", Q_RETURN_ARG(openmittsu::protocol::MessageId, returnVal), Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(QString const&, message))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke storeSentGroupMessageText from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, QString const& caption) {
			openmittsu::protocol::MessageId returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for storeSentGroupMessageImage and failed!";
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "storeSentGroupMessageImage", Q_RETURN_ARG(openmittsu::protocol::MessageId, returnVal), Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(QByteArray const&, image), Q_ARG(QString const&, caption))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke storeSentGroupMessageImage from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::utility::Location const& location) {
			openmittsu::protocol::MessageId returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for storeSentGroupMessageLocation and failed!";
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "storeSentGroupMessageLocation", Q_RETURN_ARG(openmittsu::protocol::MessageId, returnVal), Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(openmittsu::utility::Location const&, location))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke storeSentGroupMessageLocation from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QSet<openmittsu::protocol::ContactId> const& members, bool apply) {
			openmittsu::protocol::MessageId returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for storeSentGroupCreation and failed!";
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "storeSentGroupCreation", Q_RETURN_ARG(openmittsu::protocol::MessageId, returnVal), Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(QSet<openmittsu::protocol::ContactId> const&, members), Q_ARG(bool, apply))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke storeSentGroupCreation from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, bool apply) {
			openmittsu::protocol::MessageId returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for storeSentGroupSetImage and failed!";
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "storeSentGroupSetImage", Q_RETURN_ARG(openmittsu::protocol::MessageId, returnVal), Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(QByteArray const&, image), Q_ARG(bool, apply))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke storeSentGroupSetImage from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& groupTitle, bool apply) {
			openmittsu::protocol::MessageId returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for storeSentGroupSetTitle and failed!";
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "storeSentGroupSetTitle", Q_RETURN_ARG(openmittsu::protocol::MessageId, returnVal), Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(QString const&, groupTitle), Q_ARG(bool, apply))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke storeSentGroupSetTitle from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) {
			openmittsu::protocol::MessageId returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for storeSentGroupSyncRequest and failed!";
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "storeSentGroupSyncRequest", Q_RETURN_ARG(openmittsu::protocol::MessageId, returnVal), Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke storeSentGroupSyncRequest from Wrapper";
			}

			return returnVal;
		}

		openmittsu::protocol::MessageId DatabaseWrapper::storeSentGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, bool apply) {
			openmittsu::protocol::MessageId returnVal;

			auto ptr = m_database.lock();
			if (!ptr) {
				LOGGER()->error("Tried aquiring database pointer in wrapper and failed!");
				throw openmittsu::exceptions::InternalErrorException() << "Tried aquiring database pointer in wrapper for storeSentGroupLeave and failed!";
			}

			if (!QMetaObject::invokeMethod(ptr.get(), "storeSentGroupLeave", Q_RETURN_ARG(openmittsu::protocol::MessageId, returnVal), Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::protocol::MessageTime const&, timeCreated), Q_ARG(bool, isQueued), Q_ARG(bool, apply))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke storeSentGroupLeave from Wrapper";
			}

			return returnVal;
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

		std::unique_ptr<openmittsu::dataproviders::BackedContact> DatabaseWrapper::getBackedContact(openmittsu::protocol::ContactId const& contact, openmittsu::dataproviders::MessageCenter & messageCenter) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getBackedContact, std::unique_ptr<openmittsu::dataproviders::BackedContact>, Q_ARG(openmittsu::protocol::ContactId const&, contact), Q_ARG(openmittsu::dataproviders::MessageCenter&, messageCenter));
		}

		std::unique_ptr<openmittsu::dataproviders::BackedGroup> DatabaseWrapper::getBackedGroup(openmittsu::protocol::GroupId const& group, openmittsu::dataproviders::MessageCenter & messageCenter) {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getBackedGroup, std::unique_ptr<openmittsu::dataproviders::BackedGroup>, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(openmittsu::dataproviders::MessageCenter&, messageCenter));
		}

		QSet<openmittsu::protocol::ContactId> DatabaseWrapper::getGroupMembers(openmittsu::protocol::GroupId const& group, bool excludeSelfContact) const {
			OPENMITTSU_DATABASEWRAPPER_WRAP_RETURN(getGroupMembers, QSet<openmittsu::protocol::ContactId>, Q_ARG(openmittsu::protocol::GroupId const&, group), Q_ARG(bool, excludeSelfContact));
		}

		void DatabaseWrapper::enableTimers() {
			OPENMITTSU_DATABASEWRAPPER_WRAP_VOID(enableTimers);
		}

		

	}
}
