#include "src/dataproviders/BackedGroup.h"

#include <QObject>
#include "src/utility/QObjectConnectionMacro.h"

#include "src/dataproviders/BackedGroupMessage.h"
#include "src/dataproviders/MessageCenter.h"

namespace openmittsu {
	namespace dataproviders {

		BackedGroup::BackedGroup(openmittsu::protocol::GroupId const& groupId, openmittsu::database::DatabaseWrapper const& database, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter) : m_groupId(groupId), m_database(database), m_messageCenter(messageCenter), m_cursor(database.getGroupMessageCursor(groupId)) {
			OPENMITTSU_CONNECT(&dataProvider, groupChanged(openmittsu::protocol::GroupId const&), this, slotGroupChanged(openmittsu::protocol::GroupId const&));
			OPENMITTSU_CONNECT(&dataProvider, contactChanged(openmittsu::protocol::ContactId const&), this, slotIdentityChanged(openmittsu::protocol::ContactId const&));
			OPENMITTSU_CONNECT(&dataProvider, groupHasNewMessage(openmittsu::protocol::GroupId const&, QString const&), this, slotNewMessage(openmittsu::protocol::GroupId const&, QString const&));
		}

		BackedGroup::BackedGroup(BackedGroup const& other) : BackedGroup(other.m_groupId, other.m_database, other.m_messageCenter) {
			//
		}

		BackedGroup::~BackedGroup() {
			//
		}

		QString BackedGroup::getTitle() const {
			return m_database.getGroupTitle(m_groupId);
		}

		QString BackedGroup::getDescription() const {
			return m_database.getGroupDescription(m_groupId);
		}

		int BackedGroup::getMessageCount() const {
			return m_database.getGroupMessageCount(m_groupId);
		}

		openmittsu::database::MediaFileItem BackedGroup::getImage() const {
			return m_database.getGroupImage(m_groupId);
		}

		bool BackedGroup::hasImage() const {
			return m_database.getGroupHasImage(m_groupId);
		}

		bool BackedGroup::hasMember(openmittsu::protocol::ContactId const& identity) const {
			QSet<openmittsu::protocol::ContactId> const members = getMembers();
			return members.contains(identity);
		}

		openmittsu::protocol::GroupId const& BackedGroup::getId() const {
			return m_groupId;
		}

		QSet<openmittsu::protocol::ContactId> BackedGroup::getMembers() const {
			return m_database.getGroupMembers(m_groupId, false);
		}

		void BackedGroup::setGroupTitle(QString const& newTitle) {
			m_database.setGroupTitle(m_groupId, newTitle);
		}

		void BackedGroup::setGroupImage(QByteArray const& newImage) {
			m_database.setGroupImage(m_groupId, newImage);
		}

		void BackedGroup::setGroupMembers(QSet<openmittsu::protocol::ContactId> const& newMembers) {
			m_database.setGroupMembers(m_groupId, newMembers);
		}

		void BackedGroup::slotGroupChanged(openmittsu::protocol::GroupId const& changedGroupId) {
			if (changedGroupId == m_groupId) {
				emit groupDataChanged();
			}
		}

		void BackedGroup::slotIdentityChanged(openmittsu::protocol::ContactId const& changedContactId) {
			if (m_groupId.getOwner() == changedContactId || hasMember(changedContactId)) {
				emit groupDataChanged();
			}
		}

		void BackedGroup::slotNewMessage(openmittsu::protocol::GroupId const& group, QString const& messageUuid) {
			if (group == m_groupId) {
				emit newMessageAvailable(messageUuid);
			}
		}

		bool BackedGroup::sendTextMessage(QString const& text) {
			return m_messageCenter.sendText(m_groupId, text);
		}

		bool BackedGroup::sendImageMessage(QByteArray const& image, QString const& caption) {
			return m_messageCenter.sendImage(m_groupId, image, caption);
		}

		bool BackedGroup::sendLocationMessage(openmittsu::utility::Location const& location) {
			return m_messageCenter.sendLocation(m_groupId, location);
		}

		QVector<QString> BackedGroup::getLastMessageUuids(std::size_t n) {
			return m_cursor->getLastMessages(n);
		}

		BackedGroupMessage BackedGroup::getMessageByUuid(QString const& uuid) {
			return BackedGroupMessage(m_database.getGroupMessage(m_contactId, uuid), *this, m_messageCenter);
		}

		openmittsu::database::DatabaseReadonlyGroupMessage BackedGroup::fetchMessageByUuid(QString const& uuid) const {
			return m_database.getGroupMessage(m_groupId, uuid);
		}

	}
}
