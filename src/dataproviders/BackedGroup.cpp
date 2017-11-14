#include "src/dataproviders/BackedGroup.h"

#include <QObject>
#include "src/utility/QObjectConnectionMacro.h"

#include "src/dataproviders/BackedGroupMessage.h"
#include "src/dataproviders/MessageCenter.h"

namespace openmittsu {
	namespace dataproviders {

		BackedGroup::BackedGroup(openmittsu::protocol::GroupId const& groupId, GroupDataProvider& dataProvider, ContactDataProvider& contactDataProvider, openmittsu::dataproviders::MessageCenter& messageCenter) : m_groupId(groupId), m_dataProvider(dataProvider), m_contactDataProvider(contactDataProvider), m_messageCenter(messageCenter), m_cursor(dataProvider.getGroupMessageCursor(groupId)) {
			OPENMITTSU_CONNECT(&dataProvider, groupChanged(openmittsu::protocol::GroupId const&), this, slotGroupChanged(openmittsu::protocol::GroupId const&));
			OPENMITTSU_CONNECT(&dataProvider, contactChanged(openmittsu::protocol::ContactId const&), this, slotIdentityChanged(openmittsu::protocol::ContactId const&));
			OPENMITTSU_CONNECT(&dataProvider, groupHasNewMessage(openmittsu::protocol::GroupId const&, QString const&), this, slotNewMessage(openmittsu::protocol::GroupId const&, QString const&));
		}

		BackedGroup::BackedGroup(BackedGroup const& other) : BackedGroup(other.m_groupId, other.m_dataProvider, other.m_contactDataProvider, other.m_messageCenter) {
			//
		}

		BackedGroup::~BackedGroup() {
			//
		}

		QString BackedGroup::getTitle() const {
			return m_dataProvider.getGroupTitle(m_groupId);
		}

		QString BackedGroup::getDescription() const {
			return m_dataProvider.getGroupDescription(m_groupId);
		}

		int BackedGroup::getMessageCount() const {
			return m_dataProvider.getGroupMessageCount(m_groupId);
		}

		QByteArray BackedGroup::getImage() const {
			return m_dataProvider.getGroupImage(m_groupId);
		}

		bool BackedGroup::hasImage() const {
			return m_dataProvider.getGroupHasImage(m_groupId);
		}

		bool BackedGroup::hasMember(openmittsu::protocol::ContactId const& identity) const {
			QSet<openmittsu::protocol::ContactId> const members = getMembers();
			return members.contains(identity);
		}

		openmittsu::protocol::GroupId const& BackedGroup::getId() const {
			return m_groupId;
		}

		QSet<openmittsu::protocol::ContactId> BackedGroup::getMembers() const {
			return m_dataProvider.getGroupMembers(m_groupId, false);
		}

		void BackedGroup::setGroupTitle(QString const& newTitle) {
			m_dataProvider.setGroupTitle(m_groupId, newTitle);
		}

		void BackedGroup::setGroupImage(QByteArray const& newImage) {
			m_dataProvider.setGroupImage(m_groupId, newImage);
		}

		void BackedGroup::setGroupMembers(QSet<openmittsu::protocol::ContactId> const& newMembers) {
			m_dataProvider.setGroupMembers(m_groupId, newMembers);
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
			if (!m_cursor->seekByUuid(uuid)) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not return message with UUID " << uuid.toStdString() << " as it does not exist.";
			}
			auto message = m_cursor->getMessage();

			return BackedGroupMessage(message, m_contactDataProvider.getContact(message->getSender(), m_messageCenter), m_messageCenter);
		}

	}
}
