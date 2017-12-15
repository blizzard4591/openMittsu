#ifndef OPENMITTSU_DATABASE_INTERNALDATABASEINTERFACE_H_
#define OPENMITTSU_DATABASE_INTERNALDATABASEINTERFACE_H_

#include <QString>
#include <QSqlQuery>
#include <QSqlError>

namespace openmittsu {
	namespace database {
		class MediaFileItem;

		class InternalDatabaseInterface {
		public:
			virtual ~InternalDatabaseInterface() {}

			virtual QSqlQuery getQueryObject() = 0;
			virtual void announceMessageChanged(QString const& uuid) = 0;

			virtual MediaFileItem getMediaItem(QString const& uuid) = 0;
		};
	}
}

#endif // OPENMITTSU_DATABASE_INTERNALDATABASEINTERFACE_H_
