#ifndef OPENMITTSU_OPTIONS_OPTIONREADER_H_
#define OPENMITTSU_OPTIONS_OPTIONREADER_H_

#include <QObject>
#include <QSettings>
#include <QVariant>
#include <QMetaType>
#include <QString>
#include <QByteArray>
#include <QHash>
#include <QMultiHash>
#include <QtVersionChecks>

#include <memory>

#include "src/options/OptionContainer.h"
#include "src/options/OptionGroups.h"
#include "src/options/OptionRegister.h"
#include "src/options/Options.h"
#include "src/options/OptionStorage.h"
#include "src/options/OptionTypes.h"

#include "src/database/DatabaseWrapper.h"

namespace openmittsu {
	namespace options {

		class OptionReader : public QObject, public OptionRegister {
			Q_OBJECT
		public:
			OptionReader(openmittsu::database::DatabaseWrapper const& database);
			virtual ~OptionReader();

			virtual bool getOptionAsBool(Options const& option) const;
			virtual QString getOptionAsQString(Options const& option) const;
			virtual QByteArray getOptionAsQByteArray(Options const& option) const;

			void registerOptions();
			static void registerOptions(OptionRegister* target, QHash<OptionGroups, QString>& groupsToName);

			void forceInitialization();
		protected:
			virtual bool registerOption(OptionGroups const& optionGroup, Options const& option, QString const& optionName, QString const& optionDescription, QVariant const& defaultValue, OptionTypes const& optionType, OptionStorage const& optionStorage) override;
			static QSettings* getSettings();
			static QString toStringRepresentation(QVariant const& value, OptionTypes const& optionType);
			static bool toBoolRepresentation(QString const& value);
			static QString toQStringRepresentation(QString const& value);
			static QByteArray toQByteArrayRepresentation(QString const& value);
		private slots:
			void onDatabaseOptionsChanged();
			void onDatabaseUpdated();
		protected:
			void ensureOptionsExist();
			QString getOptionKeyForOption(Options const& option) const;
			OptionTypes getOptionTypeForOption(Options const& option) const;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
			QMetaType
#else
			int
#endif
			optionTypeToMetaType(OptionTypes const& type) const;

			openmittsu::database::DatabaseWrapper m_database;
			QHash<QString, QString> m_databaseCache;

			QHash<QString, Options> m_nameToOptionMap;
			QHash<Options, OptionContainer> m_optionToOptionContainerMap;
			QMultiHash<OptionGroups, Options> m_groupToOptionsMap;
			QHash<OptionGroups, QString> m_groupToNameMap;
		};

	}
}

#endif // OPENMITTSU_OPTIONS_OPTIONREADER_H_
