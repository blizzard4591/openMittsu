#include "src/backup/IdentityBackupObject.h"

#include "src/backup/IdentityBackup.h"
#include "src/exceptions/InvalidInputException.h"
#include <QFile>

namespace openmittsu {
	namespace backup {

		IdentityBackupObject::IdentityBackupObject(QString const& backupString) : BackupObject(), m_backupString(backupString) {
			if (IdentityBackup::isBackupStringMalformed(backupString)) {
				throw openmittsu::exceptions::InvalidInputException() << "The given backup string \"" << backupString.toStdString() << "\" is malformed!";
			}
		}
		
		IdentityBackupObject::~IdentityBackupObject() {
			//
		}

		QString const& IdentityBackupObject::getBackupString() const {
			return m_backupString;
		}

		IdentityBackupObject IdentityBackupObject::fromFile(QDir const& path) {
			QFile file(path.filePath(QStringLiteral("identity")));
			checkAndOpenFile(file);
			QByteArray const data = file.readAll();
			file.close();

			QString const backupString = QString::fromUtf8(data);
			return IdentityBackupObject(backupString);
		}

	}
}
