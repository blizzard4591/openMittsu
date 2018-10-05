#ifndef OPENMITTSU_DATAPROVIDERS_GROUPDATAPROVIDER_H_
#define OPENMITTSU_DATAPROVIDERS_GROUPDATAPROVIDER_H_

#include <QObject>
#include <QString>
#include <QHash>
#include <QByteArray>
#include <QSet>

#include <memory>
#include <utility>

#include "src/database/GroupData.h"
#include "src/database/NewGroupData.h"
#include "src/database/MediaFileItem.h"
#include "src/dataproviders/ContactDataProvider.h"
#include "src/dataproviders/messages/GroupMessageCursor.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/GroupStatus.h"

namespace openmittsu {
	namespace dataproviders {

		class GroupDataProvider : public ContactDataProvider {
			Q_OBJECT
		public:
			virtual ~GroupDataProvider() {}

			virtual bool hasGroup(openmittsu::protocol::GroupId const& group) const = 0;
			virtual openmittsu::protocol::GroupStatus getGroupStatus(openmittsu::protocol::GroupId const& group) const = 0;
			virtual int getGroupCount() const = 0;
			
			virtual openmittsu::database::MediaFileItem getGroupImage(openmittsu::protocol::GroupId const& group) const = 0;
			virtual QSet<openmittsu::protocol::ContactId> getGroupMembers(openmittsu::protocol::GroupId const& group, bool excludeSelfContact) const = 0;
			virtual bool getGroupIsAwaitingSync(openmittsu::protocol::GroupId const& group) const = 0;

			virtual openmittsu::database::GroupData getGroupData(openmittsu::protocol::GroupId const& group, bool withDescription) const = 0;
			virtual QHash<openmittsu::protocol::GroupId, openmittsu::database::GroupData> getGroupDataAll(bool withDescription) const = 0;

			virtual void addGroup(QVector<openmittsu::database::NewGroupData> const& newGroupData) = 0;

			virtual void setGroupTitle(openmittsu::protocol::GroupId const& group, QString const& newTitle) = 0;
			virtual void setGroupImage(openmittsu::protocol::GroupId const& group, QByteArray const& newImage) = 0;
			virtual void setGroupMembers(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& newMembers) = 0;

			virtual QSet<openmittsu::protocol::GroupId> getKnownGroups() const = 0;
			virtual QHash<openmittsu::protocol::GroupId, std::pair<QSet<openmittsu::protocol::ContactId>, QString>> getKnownGroupsWithMembersAndTitles() const = 0;
			virtual QHash<openmittsu::protocol::GroupId, QString> getKnownGroupsContainingMember(openmittsu::protocol::ContactId const& identity) const = 0;

			virtual std::shared_ptr<messages::GroupMessageCursor> getGroupMessageCursor(openmittsu::protocol::GroupId const& group) = 0;
		signals:
			void groupChanged(openmittsu::protocol::GroupId const& changedGroupId);
			void groupHasNewMessage(openmittsu::protocol::GroupId const& group, QString const& messageUuid);
		};

	}
}

Q_DECLARE_INTERFACE(openmittsu::dataproviders::GroupDataProvider, "GroupDataProvider")

#endif // OPENMITTSU_DATAPROVIDERS_GROUPDATAPROVIDER_H_
