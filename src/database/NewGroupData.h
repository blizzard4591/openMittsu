#ifndef OPENMITTSU_DATABASE_NEWGROUPDATA_H_
#define OPENMITTSU_DATABASE_NEWGROUPDATA_H_

#include "src/crypto/PublicKey.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/AccountStatus.h"
#include "src/protocol/ContactIdVerificationStatus.h"
#include "src/protocol/FeatureLevel.h"
#include "src/protocol/MessageTime.h"

#include <QMetaType>
#include <QSet>
#include <QString>

namespace openmittsu {
	namespace database {
		struct NewGroupData {
			NewGroupData() : id(0, 0), name(""), createdAt(0), members(), isDeleted(false), isAwaitingSync(false) {
				//
			}
			NewGroupData(openmittsu::protocol::GroupId const& groupId, QSet<openmittsu::protocol::ContactId> const& members, bool isAwaitingSync) : id(groupId), name(""), createdAt(openmittsu::protocol::MessageTime::now()), members(members), isDeleted(false), isAwaitingSync(isAwaitingSync) {
				//
			}
			NewGroupData(openmittsu::protocol::GroupId const& groupId, QString const& name, openmittsu::protocol::MessageTime const& createdAt, QSet<openmittsu::protocol::ContactId> const& members, bool isDeleted, bool isAwaitingSync) : id(groupId), name(name), createdAt(createdAt), members(members), isDeleted(isDeleted), isAwaitingSync(isAwaitingSync) {
				//
			}

			// openmittsu::protocol::GroupId const& groupId, QString const& name, openmittsu::protocol::MessageTime const& createdAt, QSet<openmittsu::protocol::ContactId> const& members, bool isDeleted, bool isAwaitingSync
			openmittsu::protocol::GroupId id;
			QString name;
			openmittsu::protocol::MessageTime createdAt;
			QSet<openmittsu::protocol::ContactId> members;
			bool isDeleted;
			bool isAwaitingSync;
		};
	}
}

Q_DECLARE_METATYPE(openmittsu::database::NewGroupData)

#endif // OPENMITTSU_DATABASE_NEWGROUPDATA_H_
