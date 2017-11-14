#ifndef OPENMITTSU_PROTOCOL_GROUPREGISTRY_H_
#define OPENMITTSU_PROTOCOL_GROUPREGISTRY_H_

#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"

#include <QObject>
#include <QMutex>
#include <QHash>
#include <QSet>
#include <QString>
#include <utility>

namespace openmittsu {
	namespace protocol {

		class GroupRegistry : public QObject {
			Q_OBJECT
		public:
			GroupRegistry();
			GroupRegistry(QHash<GroupId, std::pair<QSet<ContactId>, QString>> const& preKnownGroups);
			GroupRegistry(GroupRegistry const& other);
			virtual ~GroupRegistry();

			void addGroup(GroupId const& groupId, QSet<ContactId> const& members, QString const& groupTitle);

			bool hasGroup(GroupId const& groupId) const;

			QSet<ContactId> getGroupMembers(GroupId const& groupId) const;

			QString getGroupTitle(GroupId const& groupId) const;
			void setGroupTitle(GroupId const& groupId, QString const& newTitle);

			void updateGroupMembers(GroupId const& groupId, QSet<ContactId> const& members);
			void removeMember(GroupId const& groupId, ContactId const& leavingMember);
		signals:
			void groupDataChanged();
		private:
			QHash<GroupId, QSet<ContactId>> knownGroups;
			QHash<GroupId, QString> groupTitles;
			mutable QMutex mutex;
		};

	}
}

#endif // OPENMITTSU_PROTOCOL_GROUPREGISTRY_H_
