#include "src/database/DatabaseReadonlyGroupMessage.h"

#include "src/backup/GroupMessageBackupObject.h"
#include "src/database/internal/InternalDatabaseInterface.h"
#include "src/database/internal/DatabaseUtilities.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"

#include <QVariant>
#include <chrono>

namespace openmittsu {
	namespace database {

		using namespace openmittsu::dataproviders::messages;

		DatabaseReadonlyGroupMessage::DatabaseReadonlyGroupMessage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, bool isMessageFromUs, openmittsu::protocol::MessageTime const& createdAt, openmittsu::protocol::MessageTime const& sentAt, openmittsu::protocol::MessageTime const& modifiedAt, bool isQueued, bool isSent, QString const& uuid, bool isRead, bool isSaved, openmittsu::dataproviders::messages::UserMessageState const& messageState, openmittsu::protocol::MessageTime const& receivedAt, openmittsu::protocol::MessageTime const& seenAt, bool isStatusMessage, QString const& caption, openmittsu::dataproviders::messages::GroupMessageType const& groupMessageType, QString const& body, MediaFileItem const& mediaItem, MediaFileItem const& secondaryMediaItem)
			: m_group(group), m_sender(sender), m_messageId(messageId), m_isMessageFromUs(isMessageFromUs), m_createdAt(createdAt), m_sentAt(sentAt), m_modifiedAt(modifiedAt), m_isQueued(isQueued), m_isSent(isSent), m_uuid(uuid), m_isRead(isRead), m_isSaved(isSaved), m_messageState(messageState), m_receivedAt(receivedAt), m_seenAt(seenAt), m_isStatusMessage(isStatusMessage), m_caption(caption), m_groupMessageType(groupMessageType), m_body(body), m_mediaItem(mediaItem), m_secondaryMediaItem(secondaryMediaItem)
		{
			//
		}

		DatabaseReadonlyGroupMessage::~DatabaseReadonlyGroupMessage() {
			//
		}

		openmittsu::protocol::GroupId const& DatabaseReadonlyGroupMessage::getGroupId() const {
			return m_group;
		}

		openmittsu::dataproviders::messages::GroupMessageType const& DatabaseReadonlyGroupMessage::getMessageType() const {
			return m_groupMessageType;
		}

		openmittsu::protocol::ContactId const& DatabaseReadonlyGroupMessage::getSender() const {
			return m_sender;
		}

		openmittsu::protocol::MessageId const& DatabaseReadonlyGroupMessage::getMessageId() const {
			return m_messageId;
		}

		bool DatabaseReadonlyGroupMessage::isMessageFromUs() const {
			return m_isMessageFromUs;
		}

		openmittsu::protocol::MessageTime const& DatabaseReadonlyGroupMessage::getCreatedAt() const {
			return m_createdAt;
		}

		openmittsu::protocol::MessageTime const& DatabaseReadonlyGroupMessage::getSentAt() const {
			return m_sentAt;
		}

		openmittsu::protocol::MessageTime const& DatabaseReadonlyGroupMessage::getModifiedAt() const {
			return m_modifiedAt;
		}

		/** Threema Meaning unknown. We use it as follows: First, a message is only stored in the database. When we attempt to send it to the server, it is "queued". If that fails, it goes back to not queued. Else, it moves to queued and sent. */
		bool DatabaseReadonlyGroupMessage::isQueued() const {
			return m_isQueued;
		}

		/** Encodes whether this message has been successfully sent to the server. If this flag is false, the message should be resent on reconnect or timeouts. */
		bool DatabaseReadonlyGroupMessage::isSent() const {
			return m_isSent;
		}

		QString const& DatabaseReadonlyGroupMessage::getUid() const {
			return m_uuid;
		}

		bool DatabaseReadonlyGroupMessage::isRead() const {
			return m_isRead;
		}

		/** Encodes whether all data is available, i.e. referenced image or video data is available. */
		bool DatabaseReadonlyGroupMessage::isSaved() const {
			return m_isSaved;
		}

		openmittsu::dataproviders::messages::UserMessageState const& DatabaseReadonlyGroupMessage::getMessageState() const {
			return m_messageState;
		}

		openmittsu::protocol::MessageTime const& DatabaseReadonlyGroupMessage::getReceivedAt() const {
			return m_receivedAt;
		}

		openmittsu::protocol::MessageTime const& DatabaseReadonlyGroupMessage::getSeenAt() const {
			return m_seenAt;
		}

		bool DatabaseReadonlyGroupMessage::isStatusMessage() const {
			return m_isStatusMessage;
		}

		QString const& DatabaseReadonlyGroupMessage::getCaption() const {
			return m_caption;
		}

		QString const& DatabaseReadonlyGroupMessage::getContentAsText() const {
			GroupMessageType const messageType = getMessageType();
			if ((messageType != GroupMessageType::TEXT) && (messageType != GroupMessageType::AUDIO) && (messageType != GroupMessageType::SET_IMAGE) && (messageType != GroupMessageType::SET_TITLE) && (messageType != GroupMessageType::GROUP_CREATION) && (messageType != GroupMessageType::LEAVE) && (messageType != GroupMessageType::SYNC_REQUEST)) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not get content of readonly group message for message ID \"" << getMessageId().toString() << "\" as text because it has type " << GroupMessageTypeHelper::toString(messageType) << "!";
			}
			return m_body;
		}

		openmittsu::utility::Location DatabaseReadonlyGroupMessage::getContentAsLocation() const {
			GroupMessageType const messageType = getMessageType();
			if (messageType != GroupMessageType::LOCATION) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not get content of readonly group message for message ID \"" << getMessageId().toString() << "\" as location because it has type " << GroupMessageTypeHelper::toString(messageType) << "!";
			}
			return openmittsu::utility::Location::fromDatabaseString(m_body);
		}

		MediaFileItem DatabaseReadonlyGroupMessage::getContentAsMediaFile() const {
			GroupMessageType const messageType = getMessageType();
			if ((messageType != GroupMessageType::IMAGE) && (messageType != GroupMessageType::AUDIO) && (messageType != GroupMessageType::FILE) && (messageType != GroupMessageType::VIDEO)) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not get content of readonly group message for message ID \"" << getMessageId().toString() << "\" as media file because it has type " << GroupMessageTypeHelper::toString(messageType) << "!";
			}
			return m_mediaItem;
		}

		MediaFileItem DatabaseReadonlyGroupMessage::getSecondaryContentAsMediaFile() const {
			GroupMessageType const messageType = getMessageType();
			if ((messageType != GroupMessageType::FILE) && (messageType != GroupMessageType::VIDEO)) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not get secondary content of readonly group message for message ID \"" << getMessageId().toString() << "\" as media file because it has type " << GroupMessageTypeHelper::toString(messageType) << "!";
			}
			return m_secondaryMediaItem;
		}

	}
}