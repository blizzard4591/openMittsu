#include "src/dataproviders/BackedMessage.h"

#include "src/dataproviders/BackedContact.h"
#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"

#include <QMutexLocker>

namespace openmittsu {
	namespace dataproviders {

		BackedMessage::BackedMessage(QString const& uuid, BackedContact const& sender, bool isMessageFromUs, openmittsu::protocol::MessageId const& messageId) : m_uuid(uuid), m_contact(sender), m_sender(sender.getId()), m_isMessageFromUs(isMessageFromUs), m_messageId(messageId), m_cacheLoaded(false) {
			//
		}

		BackedMessage::BackedMessage(BackedMessage const& other) : m_uuid(other.m_uuid), m_contact(other.m_contact), m_sender(other.m_sender), m_isMessageFromUs(other.m_isMessageFromUs), m_messageId(other.m_messageId), m_cacheLoaded(other.m_cacheLoaded),
			m_isRead(other.m_isRead), m_isSent(other.m_isSent), m_messageState(other.m_messageState), m_timeOrderBy(other.m_timeOrderBy), m_timeCreatedAt(other.m_timeCreatedAt), m_timeSent(other.m_timeSent), m_timeReceived(other.m_timeReceived), m_timeSeen(other.m_timeSeen), m_timeModified(other.m_timeModified), m_isStatusMessage(other.m_isStatusMessage)
		{
			//
		}

		BackedMessage::~BackedMessage() {
			//
		}

		openmittsu::protocol::ContactId const& BackedMessage::getSender() const {
			return m_sender;
		}

		BackedContact const& BackedMessage::getContact() const {
			return m_contact;
		}

		bool BackedMessage::isMessageFromUs() const {
			return m_isMessageFromUs;
		}

		openmittsu::protocol::MessageId const& BackedMessage::getMessageId() const {
			return m_messageId;
		}

		bool BackedMessage::isRead() const {
			QMutexLocker mutexLock(&m_mutex);
			if (!m_cacheLoaded) { throw openmittsu::exceptions::InternalErrorException() << "Cache is not prepared!"; }
			return m_isRead;
		}

		bool BackedMessage::isSent() const {
			QMutexLocker mutexLock(&m_mutex);
			if (!m_cacheLoaded) { throw openmittsu::exceptions::InternalErrorException() << "Cache is not prepared!"; }
			return m_isSent;
		}

		messages::UserMessageState BackedMessage::getMessageState() const {
			QMutexLocker mutexLock(&m_mutex);
			if (!m_cacheLoaded) { throw openmittsu::exceptions::InternalErrorException() << "Cache is not prepared!"; }
			return m_messageState;
		}

		openmittsu::protocol::MessageTime BackedMessage::getCreatedAt() const {
			QMutexLocker mutexLock(&m_mutex);
			if (!m_cacheLoaded) { throw openmittsu::exceptions::InternalErrorException() << "Cache is not prepared!"; }
			return m_timeCreatedAt;
		}

		openmittsu::protocol::MessageTime BackedMessage::getSentAt() const {
			QMutexLocker mutexLock(&m_mutex);
			if (!m_cacheLoaded) { throw openmittsu::exceptions::InternalErrorException() << "Cache is not prepared!"; }
			return m_timeSent;
		}

		openmittsu::protocol::MessageTime BackedMessage::getReceivedAt() const {
			QMutexLocker mutexLock(&m_mutex);
			if (!m_cacheLoaded) { throw openmittsu::exceptions::InternalErrorException() << "Cache is not prepared!"; }
			return m_timeReceived;
		}

		openmittsu::protocol::MessageTime BackedMessage::getSeenAt() const {
			QMutexLocker mutexLock(&m_mutex);
			if (!m_cacheLoaded) { throw openmittsu::exceptions::InternalErrorException() << "Cache is not prepared!"; }
			return m_timeSeen;
		}

		openmittsu::protocol::MessageTime BackedMessage::getModifiedAt() const {
			QMutexLocker mutexLock(&m_mutex);
			if (!m_cacheLoaded) { throw openmittsu::exceptions::InternalErrorException() << "Cache is not prepared!"; }
			return m_timeModified;
		}

		bool BackedMessage::isStatusMessage() const {
			QMutexLocker mutexLock(&m_mutex);
			if (!m_cacheLoaded) { throw openmittsu::exceptions::InternalErrorException() << "Cache is not prepared!"; }
			return m_isStatusMessage;
		}

		QString BackedMessage::getContentAsText() const {
			return getMessage().getContentAsText();
		}

		openmittsu::utility::Location BackedMessage::getContentAsLocation() const {
			return getMessage().getContentAsLocation();
		}

		QByteArray BackedMessage::getContentAsImage() const {
			return getMessage().getContentAsImage();
		}

		QString BackedMessage::getCaption() const {
			return getMessage().getCaption();
		}

		bool BackedMessage::operator <(BackedMessage const& other) const {
			QMutexLocker mutexLock(&m_mutex);
			if (!m_cacheLoaded) { throw openmittsu::exceptions::InternalErrorException() << "Cache is not prepared!"; }

			if (m_timeCreatedAt.getMessageTime() < other.m_timeCreatedAt.getMessageTime()) {
				return true;
			} else if (m_timeCreatedAt.getMessageTime() == other.m_timeCreatedAt.getMessageTime()) {
				return m_uuid < other.m_uuid;
			} else {
				return false;
			}
		}

		bool BackedMessage::operator <=(BackedMessage const& other) const {
			QMutexLocker mutexLock(&m_mutex);
			if (!m_cacheLoaded) { throw openmittsu::exceptions::InternalErrorException() << "Cache is not prepared!"; }

			if (m_timeCreatedAt.getMessageTime() <= other.m_timeCreatedAt.getMessageTime()) {
				return true;
			} else if (m_timeCreatedAt.getMessageTime() == other.m_timeCreatedAt.getMessageTime()) {
				return m_uuid <= other.m_uuid;
			} else {
				return false;
			}
		}

		void BackedMessage::loadCache() {
			QMutexLocker mutexLock(&m_mutex);

			messages::UserMessage const& message = getMessage();
			m_isRead = message.isRead();
			m_isSent = message.isSent();
			m_messageState = message.getMessageState();
			m_timeSent = message.getSentAt();
			m_timeReceived = message.getReceivedAt();

			if (m_isMessageFromUs) {
				if (!m_isSent) {
					m_timeOrderBy = message.getCreatedAt();
				} else {
					m_timeOrderBy = m_timeSent;
				}
			} else {
				m_timeOrderBy = m_timeReceived;
			}

			m_timeCreatedAt = message.getCreatedAt();
			m_timeSeen = message.getSeenAt();
			m_timeModified = message.getModifiedAt();
			m_isStatusMessage = message.isStatusMessage();
			m_cacheLoaded = true;
		}

		void BackedMessage::onMessageChanged(QString const& uuid) {
			if (m_uuid == uuid) {
				LOGGER_DEBUG("BackedMessage: Reloading cache and announcing messageChanged() for UUID {}.", uuid.toStdString());
				loadCache();

				emit messageDataChanged();
			}
		}

	}
}
