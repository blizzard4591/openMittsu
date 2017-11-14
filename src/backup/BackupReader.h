#ifndef OPENMITTSU_BACKUP_BACKUPREADER_H_
#define OPENMITTSU_BACKUP_BACKUPREADER_H_

#include "src/database/Database.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/crypto/KeyPair.h"

#include <memory>

#include <QDir>
#include <QHash>
#include <QString>
#include <QObject>
#include <cstdint>

namespace openmittsu {
	namespace backup {

		class BackupReader : public QThread {
			Q_OBJECT
		public:
			BackupReader(QDir const& backupFilePath, QString const& backupPassword, QString const& databaseFilename, QDir const& mediaStorageLocation, QString const& databasePassword);
			virtual ~BackupReader();

			virtual void run() override;
		signals:
			void progressUpdated(int percentComplete);
			void finished(bool hadError, QString const& errorMessage);
		private:
			QDir const m_backupFilePath;
			QString const m_backupPassword;
			QString const m_databaseFilename;
			QDir const m_mediaStorageLocation;
			QString const m_databasePassword;
		};

	}
}

#endif // OPENMITTSU_BACKUP_BACKUPREADER_H_
