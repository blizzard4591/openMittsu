#ifndef OPENMITTSU_BACKUP_IDENTITYBACKUPOBJECT_H_
#define OPENMITTSU_BACKUP_IDENTITYBACKUPOBJECT_H_

#include "src/backup/BackupObject.h"

#include <QDir>
#include <QString>

namespace openmittsu {
	namespace backup {

		class IdentityBackupObject : public BackupObject {
		public:
			IdentityBackupObject(QString const& backupString);
			virtual ~IdentityBackupObject();
			
			QString const& getBackupString() const;

			static IdentityBackupObject fromFile(QDir const& path);
		private:
			QString const m_backupString;
		};

	}
}

#endif // OPENMITTSU_BACKUP_IDENTITYBACKUPOBJECT_H_
