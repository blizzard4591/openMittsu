#ifndef OPENMITTSU_BACKUP_BACKUPOBJECT_H_
#define OPENMITTSU_BACKUP_BACKUPOBJECT_H_

#include <QHash>
#include <QFile>
#include <QSet>
#include <QString>

namespace openmittsu {
	namespace backup {

		class BackupObject {
		public:
			BackupObject();
			virtual ~BackupObject();
		protected:
			static void checkAndOpenFile(QFile& file);
			static bool hasRequiredFields(QSet<QString> const& requiredFields, QHash<QString, int> const& headerOffsets);
		};

	}
}

#endif // OPENMITTSU_BACKUP_BACKUPOBJECT_H_
