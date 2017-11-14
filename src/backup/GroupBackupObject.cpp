#include "src/backup/GroupBackupObject.h"

#include "src/exceptions/IllegalArgumentException.h"

#include "src/utility/Logging.h"

namespace openmittsu {
	namespace backup {

		GroupBackupObject::GroupBackupObject(openmittsu::protocol::GroupId const& id, QString const& name, openmittsu::protocol::MessageTime const& createdAt, QSet<openmittsu::protocol::ContactId> const& members, bool isDeleted) : BackupObject(),
			m_id(id), m_name(name), m_createdAt(createdAt), m_members(members), m_isDeleted(isDeleted) {
			//
		}
		
		GroupBackupObject::~GroupBackupObject() {
			//
		}

		openmittsu::protocol::GroupId const& GroupBackupObject::getGroupId() const {
			return m_id;
		}
		
		QString const& GroupBackupObject::getName() const {
			return m_name;
		}
		
		openmittsu::protocol::MessageTime const& GroupBackupObject::getCreatedAt() const {
			return m_createdAt;
		}
		
		QSet<openmittsu::protocol::ContactId> const& GroupBackupObject::getMembers() const {
			return m_members;
		}
		
		bool GroupBackupObject::getIsDeleted() const {
			return m_isDeleted;
		}

		GroupBackupObject GroupBackupObject::fromBackupMatch(QString const&, QHash<QString, int> const& headerOffsets, SimpleCsvLineSplitter const& splittedLines) {
			QSet<QString> const requiredFields = { QStringLiteral("id"), QStringLiteral("creator"), QStringLiteral("groupname"), QStringLiteral("created_at"), QStringLiteral("members"), QStringLiteral("deleted") };
			if (!hasRequiredFields(requiredFields, headerOffsets)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Could not parse data to Group, not all required fields are present!";
			} else {
				openmittsu::protocol::ContactId const owner(openmittsu::protocol::ContactId(splittedLines.getColumn(headerOffsets.value(QStringLiteral("creator")))));
				openmittsu::protocol::GroupId const id(owner, splittedLines.getColumn(headerOffsets.value(QStringLiteral("id"))));
				QString const name(splittedLines.getColumn(headerOffsets.value(QStringLiteral("groupname"))));
				openmittsu::protocol::MessageTime const createdAt = openmittsu::protocol::MessageTime::fromDatabase(splittedLines.getColumn(headerOffsets.value(QStringLiteral("created_at"))).toLongLong());
				QString const memberString(splittedLines.getColumn(headerOffsets.value(QStringLiteral("members"))));
				QStringList const memberList = memberString.split(';', QString::SkipEmptyParts);
				QSet<openmittsu::protocol::ContactId> members;
				for (int i = 0; i < memberList.size(); ++i) {
					members.insert(openmittsu::protocol::ContactId(memberList.at(i)));
				}
				bool const isDeleted = splittedLines.getColumn(headerOffsets.value(QStringLiteral("deleted"))).toInt() != 0;

				return GroupBackupObject(id, name, createdAt, members, isDeleted);
			}
		}

	}
}
