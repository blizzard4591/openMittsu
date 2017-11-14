#ifndef OPENMITTSU_BACKUP_GROUPBACKUPOBJECT_H_
#define OPENMITTSU_BACKUP_GROUPBACKUPOBJECT_H_

#include "src/backup/BackupObject.h"
#include "src/backup/SimpleCsvLineSplitter.h"

#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/MessageTime.h"
#include "src/crypto/PublicKey.h"
#include "src/protocol/ContactIdVerificationStatus.h"

namespace openmittsu {
	namespace backup {

		class GroupBackupObject : public BackupObject {
		public:
			GroupBackupObject(openmittsu::protocol::GroupId const& id, QString const& name, openmittsu::protocol::MessageTime const& createdAt, QSet<openmittsu::protocol::ContactId> const& members, bool isDeleted);
			virtual ~GroupBackupObject();

			openmittsu::protocol::GroupId const& getGroupId() const;
			QString const& getName() const;
			openmittsu::protocol::MessageTime const& getCreatedAt() const;
			QSet<openmittsu::protocol::ContactId> const& getMembers() const;
			bool getIsDeleted() const;

			static GroupBackupObject fromBackupMatch(QString const& filename, QHash<QString, int> const& headerOffsets, SimpleCsvLineSplitter const& splittedLines);
		private:
			openmittsu::protocol::GroupId m_id;
			QString m_name;
			openmittsu::protocol::MessageTime m_createdAt;
			QSet<openmittsu::protocol::ContactId> m_members;
			bool m_isDeleted;
		};

	}
}

#endif // OPENMITTSU_BACKUP_GROUPBACKUPOBJECT_H_
