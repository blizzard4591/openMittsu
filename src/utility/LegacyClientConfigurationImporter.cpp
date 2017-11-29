#include "src/utility/LegacyClientConfigurationImporter.h"

#include "exceptions/IllegalArgumentException.h"
#include "exceptions/InternalErrorException.h"
#include "utility/Logging.h"
#include "utility/QObjectConnectionMacro.h"

#include <QFile>
#include <QSettings>

namespace openmittsu {
	namespace utility {

		LegacyClientConfigurationImporter::LegacyClientConfigurationImporter(openmittsu::backup::IdentityBackup const& identityBackup, QString const& backupString, QString const& backupPassword) : QObject(), m_identityBackup(identityBackup), m_backupString(backupString), m_backupPassword(backupPassword) {
			//
		}

		LegacyClientConfigurationImporter::LegacyClientConfigurationImporter(LegacyClientConfigurationImporter const& other) : QObject(), m_identityBackup(other.m_identityBackup), m_backupString(other.m_backupString), m_backupPassword(other.m_backupPassword) {
			//
		}

		LegacyClientConfigurationImporter::~LegacyClientConfigurationImporter() {
			//
		}
		
		LegacyClientConfigurationImporter LegacyClientConfigurationImporter::fromFile(QString const& filename) {
			if (!QFile::exists(filename)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Could not open ClientConfiguration file, file does not exist: " << filename.toStdString();
			}
			QFile inputFile(filename);
			if (!inputFile.open(QFile::ReadOnly)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Could not open ClientConfiguration file for reading: " << filename.toStdString();
			}
			inputFile.close();

			QSettings clientSettings(filename, QSettings::IniFormat);

			if (!clientSettings.contains("clientIdentityBackupPassword")) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Invalid ClientConfiguration: Missing clientIdentityBackupPassword in " << filename.toStdString();
			}

			if (!clientSettings.contains("clientIdentityBackupString")) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Invalid ClientConfiguration: Missing clientIdentityBackupString in " << filename.toStdString();
			}

			QString const backupString(clientSettings.value("clientIdentityBackupString").toString());
			QString const backupPassword(clientSettings.value("clientIdentityBackupPassword").toString());
			openmittsu::backup::IdentityBackup identityBackup(openmittsu::backup::IdentityBackup::fromBackupString(backupString, backupPassword));

			return LegacyClientConfigurationImporter(identityBackup, backupString, backupPassword);
		}

		openmittsu::backup::IdentityBackup const& LegacyClientConfigurationImporter::getIdentityBackup() const {
			return m_identityBackup;
		}

		QString const& LegacyClientConfigurationImporter::getBackupString() const {
			return m_backupString;
		}

		QString const& LegacyClientConfigurationImporter::getBackupPassword() const {
			return m_backupPassword;
		}

	}
}
