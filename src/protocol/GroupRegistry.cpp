#include "src/protocol/GroupRegistry.h"

#include "src/exceptions/CryptoException.h"
#include "src/exceptions/IllegalArgumentException.h"

namespace openmittsu {
	namespace protocol {

		GroupRegistry::GroupRegistry() {
			//
		}

		GroupRegistry::GroupRegistry(QHash<GroupId, std::pair<QSet<ContactId>, QString>> const& preKnownGroups) : mutex() {
			QHash<GroupId, std::pair<QSet<ContactId>, QString>>::const_iterator it = preKnownGroups.constBegin();
			QHash<GroupId, std::pair<QSet<ContactId>, QString>>::const_iterator end = preKnownGroups.constEnd();

			for (; it != end; ++it) {
				knownGroups.insert(it.key(), it.value().first);
				groupTitles.insert(it.key(), it.value().second);
			}
		}

		GroupRegistry::GroupRegistry(GroupRegistry const& other) : knownGroups(other.knownGroups), groupTitles(other.groupTitles), mutex() {
			// Intentionally left empty.
		}

		GroupRegistry::~GroupRegistry() {
			// Intentionally left empty.
		}

		void GroupRegistry::addGroup(GroupId const& groupId, QSet<ContactId> const& members, QString const& groupTitle) {
			if (!hasGroup(groupId)) {
				mutex.lock();
				if (!members.contains(groupId.getOwner())) {
					QSet<ContactId> fullMembers(members);
					fullMembers.insert(groupId.getOwner());
					knownGroups.insert(groupId, fullMembers);
				} else {
					knownGroups.insert(groupId, members);
				}
				groupTitles.insert(groupId, groupTitle);
				mutex.unlock();

				emit groupDataChanged();
			}
		}

		void GroupRegistry::updateGroupMembers(GroupId const& groupId, QSet<ContactId> const& members) {
			if (hasGroup(groupId)) {
				mutex.lock();
				if (!members.contains(groupId.getOwner())) {
					QSet<ContactId> fullMembers(members);
					fullMembers.insert(groupId.getOwner());
					knownGroups.insert(groupId, fullMembers);
				} else {
					knownGroups.insert(groupId, members);
				}
				mutex.unlock();

				emit groupDataChanged();
			}
		}

		void GroupRegistry::setGroupTitle(GroupId const& groupId, QString const& newTitle) {
			if (hasGroup(groupId)) {
				mutex.lock();
				groupTitles.insert(groupId, newTitle);
				mutex.unlock();

				emit groupDataChanged();
			}
		}

		void GroupRegistry::removeMember(GroupId const& groupId, ContactId const& leavingMember) {
			if (hasGroup(groupId)) {
				mutex.lock();
				QSet<ContactId> oldMembers = knownGroups.value(groupId);
				oldMembers.remove(leavingMember);

				knownGroups.insert(groupId, oldMembers);
				mutex.unlock();

				emit groupDataChanged();
			}
		}

		bool GroupRegistry::hasGroup(GroupId const& groupId) const {
			mutex.lock();
			bool result = knownGroups.contains(groupId);
			mutex.unlock();

			return result;
		}

		QSet<ContactId> GroupRegistry::getGroupMembers(GroupId const& groupId) const {
			if (!hasGroup(groupId)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "The given group does not exist.";
			}

			mutex.lock();
			QSet<ContactId> result = knownGroups.value(groupId);
			mutex.unlock();

			return result;
		}

		QString GroupRegistry::getGroupTitle(GroupId const& groupId) const {
			if (hasGroup(groupId)) {
				if (groupTitles.contains(groupId)) {
					return groupTitles.value(groupId);
				} else {
					return QString("");
				}
			} else {
				throw openmittsu::exceptions::IllegalArgumentException() << "The given group does not exist.";
			}
		}

	}
}
