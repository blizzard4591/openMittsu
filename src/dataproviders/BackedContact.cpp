#include "src/dataproviders/BackedContact.h"

#include "src/dataproviders/BackedContactMessage.h"
#include "src/utility/QObjectConnectionMacro.h"

#include "MessageCenter.h"

#include "src/dataproviders/MessageCenter.h"

namespace openmittsu {
	namespace dataproviders {

		BackedContact::BackedContact(openmittsu::protocol::ContactId const& contactId, openmittsu::crypto::PublicKey const& contactPublicKey, openmittsu::database::DatabaseWrapper const& database, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter) : m_contactId(contactId), m_contactPublicKey(contactPublicKey), m_database(database), m_messageCenter(messageCenter), m_cursor(m_database.getContactMessageCursor(contactId)) {
			OPENMITTSU_CONNECT(&m_database, contactChanged(openmittsu::protocol::ContactId const&), this, slotIdentityChanged(openmittsu::protocol::ContactId const&));
			OPENMITTSU_CONNECT(&m_database, contactHasNewMessage(openmittsu::protocol::ContactId const&, QString const&), this, slotNewMessage(openmittsu::protocol::ContactId const&, QString const&));
			OPENMITTSU_CONNECT(&m_database, contactStartedTyping(openmittsu::protocol::ContactId const&), this, slotContactStartedTyping(openmittsu::protocol::ContactId const&));
			OPENMITTSU_CONNECT(&m_database, contactStoppedTyping(openmittsu::protocol::ContactId const&), this, slotContactStoppedTyping(openmittsu::protocol::ContactId const&));
		}

		BackedContact::BackedContact(BackedContact const& other) : BackedContact(other.m_contactId, other.m_contactPublicKey, other.m_database, other.m_messageCenter) {
			//
		}

		BackedContact::~BackedContact() {
			//
		}

		QString BackedContact::getName() const {
			QString const nickname = getNickname();
			if (nickname.isNull() || nickname.isEmpty()) {
				return m_contactId.toQString();
			} else {
				return QStringLiteral("%1 (%2)").arg(nickname).arg(m_contactId.toQString());
			}
		}

		QString BackedContact::getFirstName() const {
			return m_database.getFirstName(m_contactId);
		}

		QString BackedContact::getLastName() const {
			return m_database.getLastName(m_contactId);
		}

		void BackedContact::setNickname(QString const& newNickname) {
			m_database.setNickName(getId(), newNickname);
		}

		void BackedContact::setFirstName(QString const& newFirstName) {
			m_database.setFirstName(m_contactId, newFirstName);
		}

		void BackedContact::setLastName(QString const& newLastName) {
			m_database.setLastName(m_contactId, newLastName);
		}

		QString BackedContact::getNickname() const {
			return m_database.getNickName(m_contactId);
		}

		openmittsu::crypto::PublicKey const& BackedContact::getPublicKey() const {
			return m_contactPublicKey;
		}

		openmittsu::protocol::ContactId const& BackedContact::getId() const {
			return m_contactId;
		}

		openmittsu::protocol::AccountStatus BackedContact::getActivityStatus() const {
			return m_database.getAccountStatus(m_contactId);
		}

		int BackedContact::getMessageCount() const {
			return m_database.getContactMessageCount(m_contactId);
		}

		void BackedContact::slotIdentityChanged(openmittsu::protocol::ContactId const& changedContactId) {
			if (m_contactId == changedContactId) {
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
			return BackedContactMessage(*m_database.getContactMessage(m_contactId, uuid), *this, m_messageCenter);
		}

		openmittsu::database::DatabaseReadonlyContactMessage BackedContact::fetchMessageByUuid(QString const& uuid) const {
			return *m_database.getContactMessage(m_contactId, uuid);
		}

	}
}
