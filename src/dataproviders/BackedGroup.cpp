#include "src/dataproviders/BackedGroup.h"

#include <QObject>
#include "src/utility/QObjectConnectionMacro.h"

#include "src/dataproviders/BackedGroupMessage.h"
#include "src/dataproviders/MessageCenter.h"

namespace openmittsu {
	namespace dataproviders {

		BackedGroup::BackedGroup(openmittsu::protocol::GroupId const& groupId, openmittsu::database::DatabaseWrapper const& database, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter, BackedContactAndGroupPool& pool) : m_groupId(groupId), m_database(database), m_messageCenter(messageCenter), m_pool(pool), m_groupData(m_database.getGroupData(m_groupId, true)) {
			OPENMITTSU_CONNECT(&m_database, groupChanged(openmittsu::protocol::GroupId const&), this, slotGroupChanged(openmittsu::protocol::GroupId const&));
			OPENMITTSU_CONNECT(&m_database, contactChanged(openmittsu::protocol::ContactId const&), this, slotIdentityChanged(openmittsu::protocol::ContactId const&));
			OPENMITTSU_CONNECT(&m_database, groupHasNewMessage(openmittsu::protocol::GroupId const&, QString const&), this, slotNewMessage(openmittsu::protocol::GroupId const&, QString const&));
		}

		BackedGroup::BackedGroup(BackedGroup const& other) : BackedGroup(other.m_groupId, other.m_database, other.m_messageCenter, other.m_pool) {
			//
		}

		BackedGroup::~BackedGroup() {
			//
		}

		QString BackedGroup::getTitle() const {
			return m_groupData.title;
		}

		QString BackedGroup::getDescription() const {
			return m_groupData.description;
		}

		int BackedGroup::getMessageCount() const {
			return m_groupData.messageCount;
		}

		openmittsu::database::MediaFileItem BackedGroup::getImage() const {
			return m_groupData.image;
		}

		bool BackedGroup::hasImage() const {
			return m_groupData.hasImage;
		}

		bool BackedGroup::hasMember(openmittsu::protocol::ContactId const& identity) const {
			return m_groupData.members.contains(identity);
		}

		openmittsu::protocol::GroupId const& BackedGroup::getId() const {
			return m_groupId;
		}

		QSet<openmittsu::protocol::ContactId> BackedGroup::getMembers() const {
			return m_groupData.members;
		}

		void BackedGroup::slotGroupChanged(openmittsu::protocol::GroupId const& changedGroupId) {
			if (changedGroupId == m_groupId) {
				m_groupData = m_database.getGroupData(m_groupId, true);
				emit groupDataChanged();
			}
		}

		void BackedGroup::slotIdentityChanged(openmittsu::protocol::ContactId const& changedContactId) {
			if (m_groupId.getOwner() == changedContactId || hasMember(changedContactId)) {
				m_groupData = m_database.getGroupData(m_groupId, true);
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
			return m_database.getLastMessageUuids(m_groupId, n);
		}

		BackedGroupMessage BackedGroup::getMessageByUuid(QString const& uuid) {
			auto message = m_database.getGroupMessage(m_groupId, uuid);
			return BackedGroupMessage(*message, m_pool.getBackedContact(message->getSender(), m_database, m_messageCenter), m_pool.getBackedGroup(m_groupId, m_database, m_messageCenter), m_messageCenter);
		}

		openmittsu::database::DatabaseReadonlyGroupMessage BackedGroup::fetchMessageByUuid(QString const& uuid) {
			return *m_database.getGroupMessage(m_groupId, uuid);
		}

	}
}
