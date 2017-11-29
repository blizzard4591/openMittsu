#ifndef OPENMITTSU_UTILITY_LEGACYCLIENTCONFIGURATIONIMPORTER_H_
#define OPENMITTSU_UTILITY_LEGACYCLIENTCONFIGURATIONIMPORTER_H_

#include <QHash>
#include <QObject>
#include <QString>

#include <cstdint>
#include <utility>

#include "src/backup/IdentityBackup.h"

namespace openmittsu {
	namespace utility {

		class LegacyClientConfigurationImporter : public QObject {
			Q_OBJECT
		public:
			LegacyClientConfigurationImporter(LegacyClientConfigurationImporter const& other);
			virtual ~LegacyClientConfigurationImporter();
			static LegacyClientConfigurationImporter fromFile(QString const& filename);

			openmittsu::backup::IdentityBackup const& getIdentityBackup() const;
			QString const& getBackupString() const;
			QString const& getBackupPassword() const;
		private:
			LegacyClientConfigurationImporter(openmittsu::backup::IdentityBackup const& identityBackup, QString const& backupString, QString const& backupPassword);

			openmittsu::backup::IdentityBackup const m_identityBackup;
			QString const m_backupString;
			QString const m_backupPassword;
		};

	}
}

#endif // OPENMITTSU_UTILITY_LEGACYCLIENTCONFIGURATIONIMPORTER_H_