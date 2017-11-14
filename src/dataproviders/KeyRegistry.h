#ifndef OPENMITTSU_DATAPROVIDERS_KEYREGISTRY_H_
#define OPENMITTSU_DATAPROVIDERS_KEYREGISTRY_H_

#include "src/crypto/PublicKey.h"
#include "src/crypto/KeyPair.h"
#include "src/protocol/ContactId.h"

#include <QObject>
#include <QMutex>
#include <QHash>

#include <memory>

namespace openmittsu {
	namespace database {
		class Database;
	}

	namespace dataproviders {

		class KeyRegistry : public QObject {
			Q_OBJECT
		public:
			KeyRegistry(openmittsu::crypto::PublicKey const& serverLongTermPublicKey, std::weak_ptr<openmittsu::database::Database> const& database);
			KeyRegistry(KeyRegistry const& other);
			virtual ~KeyRegistry();

			bool hasIdentity(openmittsu::protocol::ContactId const& identity) const;
			openmittsu::crypto::PublicKey getPublicKeyForIdentity(openmittsu::protocol::ContactId const& identity) const;

			openmittsu::crypto::KeyPair const& getClientLongTermKeyPair() const;
			openmittsu::crypto::PublicKey const& getServerLongTermPublicKey() const;

			openmittsu::protocol::ContactId getSelfContactId() const;
		private slots:
			void onContactChanged();
		private:
			KeyRegistry();
			void invalidateCache();
			void updateCache();

			mutable QMutex m_mutex;
			bool m_isCacheValid;

			openmittsu::protocol::ContactId m_cachedSelfContactId;
			openmittsu::crypto::KeyPair m_cachedClientLongTermKeyPair;
			openmittsu::crypto::PublicKey const m_serverLongTermPublicKey;

			QHash<openmittsu::protocol::ContactId, openmittsu::crypto::PublicKey> m_cachedPublicKeys;

			std::weak_ptr<openmittsu::database::Database> const m_database;
		};

	}
}

#endif // OPENMITTSU_DATAPROVIDERS_KEYREGISTRY_H_
