#include "src/dataproviders/BackedMessage.h"

#include "src/dataproviders/BackedContact.h"
#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"

#include <QMutexLocker>

namespace openmittsu {
	namespace dataproviders {

		BackedMessage::BackedMessage(QString const& uuid, std::shared_ptr<BackedContact> const& sender, bool isMessageFromUs, openmittsu::protocol::MessageId const& messageId) : m_uuid(uuid), m_contact(sender), m_isMessageFromUs(isMessageFromUs), m_isDeleted(false) {
			//
		}

		BackedMessage::BackedMessage(BackedMessage const& other) : m_uuid(other.m_uuid), m_contact(other.m_contact), m_isMessageFromUs(other.m_isMessageFromUs), m_isDeleted(other.m_isDeleted) {
			//
		}

		BackedMessage::~BackedMessage() {
			//
		}

		openmittsu::protocol::ContactId const& BackedMessage::getSender() const {
			return getMessage().getSender();
		}

		std::shared_ptr<BackedContact> const& BackedMessage::getContact() const {
			return m_contact;
		}

		bool BackedMessage::isMessageFromUs() const {
			return m_isMessageFromUs;
		}

		openmittsu::protocol::MessageId const& BackedMessage::getMessageId() const {
			return getMessage().getMessageId();
		}

		bool BackedMessage::isRead() const {
			return getMessage().isRead();
		}

		bool BackedMessage::isSent() const {
			return getMessage().isSent();
		}

		messages::UserMessageState const& BackedMessage::getMessageState() const {
			return getMessage().getMessageState();
		}

		openmittsu::protocol::MessageTime const& BackedMessage::getCreatedAt() const {
			return getMessage().getCreatedAt();
		}

		openmittsu::protocol::MessageTime const& BackedMessage::getSentAt() const {
			return getMessage().getSentAt();
		}

		openmittsu::protocol::MessageTime const& BackedMessage::getReceivedAt() const {
			return getMessage().getReceivedAt();
		}

		openmittsu::protocol::MessageTime const& BackedMessage::getSeenAt() const {
			return getMessage().getSeenAt();
		}

		openmittsu::protocol::MessageTime const& BackedMessage::getModifiedAt() const {
			return getMessage().getModifiedAt();
		}

		bool BackedMessage::isStatusMessage() const {
			return getMessage().isStatusMessage();
		}

		QString BackedMessage::getContentAsText() const {
			return getMessage().getContentAsText();
		}

		openmittsu::utility::Location BackedMessage::getContentAsLocation() const {
			return getMessage().getContentAsLocation();
		}

		openmittsu::database::MediaFileItem BackedMessage::getContentAsMediaFile() const {
			return getMessage().getContentAsMediaFile();
		}

		QString const& BackedMessage::getCaption() const {
			return getMessage().getCaption();
		}

		bool BackedMessage::operator <(BackedMessage const& other) const {
			if (getCreatedAt().getMessageTime() < other.getCreatedAt().getMessageTime()) {
				return true;
			} else if (getCreatedAt().getMessageTime() == other.getCreatedAt().getMessageTime()) {
				return m_uuid < other.m_uuid;
			} else {
				return false;
			}
		}

		bool BackedMessage::operator <=(BackedMessage const& other) const {
			if (getCreatedAt().getMessageTime() <= other.getCreatedAt().getMessageTime()) {
				return true;
			} else if (getCreatedAt().getMessageTime() == other.getCreatedAt().getMessageTime()) {
				return m_uuid <= other.m_uuid;
			} else {
				return false;
			}
		}

		void BackedMessage::onMessageChanged(QString const& uuid) {
			if (m_uuid == uuid) {
				LOGGER_DEBUG("BackedMessage: Reloading cache and announcing messageChanged() for UUID {}.", uuid.toStdString());
				loadCache();

				emit messageDataChanged();
			}
		}

		void BackedMessage::onMessageDeleted(QString const& uuid) {
			if (m_uuid == uuid) {
				LOGGER_DEBUG("BackedMessage: Announcing messageDeleted() for UUID {}.", uuid.toStdString());
				m_isDeleted = true;
				
				emit messageDeleted();
			}
		}

		void BackedMessage::deleteMessage() {
			m_contact->deleteMessageByUuid(m_uuid);

			m_isDeleted = true;
		}

	}
}
