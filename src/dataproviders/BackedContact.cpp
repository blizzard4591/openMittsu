#include "src/dataproviders/BackedContact.h"

#include "src/dataproviders/BackedContactMessage.h"
#include "src/utility/QObjectConnectionMacro.h"

#include "MessageCenter.h"

#include "src/dataproviders/MessageCenter.h"

namespace openmittsu {
	namespace dataproviders {

		BackedContact::BackedContact(openmittsu::protocol::ContactId const& contactId, openmittsu::database::DatabaseWrapper const& database, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter, BackedContactAndGroupPool& pool) : m_contactId(contactId), m_database(database), m_messageCenter(messageCenter), m_pool(pool), m_contactData(m_database.getContactData(m_contactId, true)) {
			OPENMITTSU_CONNECT(&m_database, contactChanged(openmittsu::protocol::ContactId const&), this, slotIdentityChanged(openmittsu::protocol::ContactId const&));
			OPENMITTSU_CONNECT(&m_database, contactHasNewMessage(openmittsu::protocol::ContactId const&, QString const&), this, slotNewMessage(openmittsu::protocol::ContactId const&, QString const&));
			OPENMITTSU_CONNECT(&m_database, contactStartedTyping(openmittsu::protocol::ContactId const&), this, slotContactStartedTyping(openmittsu::protocol::ContactId const&));
			OPENMITTSU_CONNECT(&m_database, contactStoppedTyping(openmittsu::protocol::ContactId const&), this, slotContactStoppedTyping(openmittsu::protocol::ContactId const&));
		}

		BackedContact::BackedContact(BackedContact const& other) : BackedContact(other.m_contactId, other.m_database, other.m_messageCenter, other.m_pool) {
			//
		}

		BackedContact::~BackedContact() {
			//
		}

		QString BackedContact::getName() const {
			return m_contactData.nickName;
		}

		QString BackedContact::getFirstName() const {
			return m_contactData.firstName;
		}

		QString BackedContact::getLastName() const {
			return m_contactData.lastName;
		}

		void BackedContact::setNickname(QString const& newNickname) {
			m_database.setContactNickName(getId(), newNickname);
		}

		void BackedContact::setFirstName(QString const& newFirstName) {
			m_database.setContactFirstName(m_contactId, newFirstName);
		}

		void BackedContact::setLastName(QString const& newLastName) {
			m_database.setContactLastName(m_contactId, newLastName);
		}

		QString BackedContact::getNickname() const {
			return m_contactData.nickName;
		}

		openmittsu::crypto::PublicKey const& BackedContact::getPublicKey() const {
			return m_contactData.publicKey;
		}

		openmittsu::protocol::ContactId const& BackedContact::getId() const {
			return m_contactId;
		}

		openmittsu::protocol::AccountStatus BackedContact::getActivityStatus() const {
			return m_contactData.accountStatus;
		}

		int BackedContact::getMessageCount() const {
			return m_contactData.messageCount;
		}

		void BackedContact::slotIdentityChanged(openmittsu::protocol::ContactId const& changedContactId) {
			if (m_contactId == changedContactId) {
				m_contactData = m_database.getContactData(m_contactId, true);
				emit contactDataChanged();
			}
		}

		void BackedContact::slotNewMessage(openmittsu::protocol::ContactId const& contactId, QString const& messageUuid) {
			if (m_contactId == contactId) {
				emit newMessageAvailable(messageUuid);
			}
		}

		void BackedContact::slotContactStartedTyping(openmittsu::protocol::ContactId const& contactId) {
			if (m_contactId == contactId) {
				emit contactStartedTyping();
			}
		}

		void BackedContact::slotContactStoppedTyping(openmittsu::protocol::ContactId const& contactId) {
			if (m_contactId == contactId) {
				emit contactStoppedTyping();
			}
		}

		bool BackedContact::sendTextMessage(QString const& text) {
			return m_messageCenter.sendText(m_contactId, text);
		}

		bool BackedContact::sendImageMessage(QByteArray const& image, QString const& caption) {
			return m_messageCenter.sendImage(m_contactId, image, caption);
		}

		bool BackedContact::sendLocationMessage(openmittsu::utility::Location const& location) {
			return m_messageCenter.sendLocation(m_contactId, location);
		}

		void BackedContact::sendTypingNotification(bool typingStopped) {
			m_messageCenter.sendUserTypingStatus(m_contactId, !typingStopped);
		}

		QVector<QString> BackedContact::getLastMessageUuids(std::size_t n) {
			return m_database.getLastMessageUuids(m_contactId, n);
		}

		BackedContactMessage BackedContact::getMessageByUuid(QString const& uuid) {
			return BackedContactMessage(*m_database.getContactMessage(m_contactId, uuid), m_pool.getBackedContact(m_contactId, m_database, m_messageCenter), m_messageCenter);
		}

		openmittsu::database::DatabaseReadonlyContactMessage BackedContact::fetchMessageByUuid(QString const& uuid) {
			return *m_database.getContactMessage(m_contactId, uuid);
		}

	}
}
