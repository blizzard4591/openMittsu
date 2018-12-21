#ifndef OPENMITTSU_DATABASE_DATABASETHREADWORKER_H_
#define OPENMITTSU_DATABASE_DATABASETHREADWORKER_H_

#include <QDir>
#include <QObject>
#include <QString>
#include <QThread>

#include <memory>

#include "src/crypto/KeyPair.h"
#include "src/database/Database.h"
#include "src/protocol/ContactId.h"

namespace openmittsu {
	namespace database {

		enum class DatabaseOpenFailureReason {
			FREASON_NO_ERROR,
			FREASON_INVALID_PASSWORD,
			FREASON_UNKNOWN,
			FREASON_OTHER
		};

		struct DatabaseOpenResult {
			bool success;
			DatabaseOpenFailureReason failureReason;
		};

		class DatabaseThreadWorker : public QObject {
			Q_OBJECT
		public:
			virtual ~DatabaseThreadWorker();
		public slots:
			int openDatabase(QString const& filename, QString const& password, QDir const& mediaStorageLocation, bool useCompatibilityToVersionThree);
			bool createDatabase(QString const& filename, openmittsu::protocol::ContactId const& selfContact, openmittsu::crypto::KeyPair const& selfLongTermKeyPair, QString const& password, QDir const& mediaStorageLocation);

			bool hasDatabase() const;

			std::shared_ptr<openmittsu::database::Database> getDatabase();
			std::shared_ptr<openmittsu::database::Database> const& getDatabase() const;
		private:
			std::shared_ptr<openmittsu::database::Database> m_database;
		};

	}
}

Q_DECLARE_METATYPE(openmittsu::database::DatabaseOpenResult)

#endif // OPENMITTSU_DATABASE_DATABASETHREADWORKER_H_
