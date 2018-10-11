#ifndef OPENMITTSU_DATAPROVIDERS_KEYREGISTRY_H_
#define OPENMITTSU_DATAPROVIDERS_KEYREGISTRY_H_

#include "src/crypto/PublicKey.h"
#include "src/crypto/KeyPair.h"
#include "src/database/ContactData.h"
#include "src/database/DatabaseWrapper.h"
#include "src/protocol/ContactId.h"

#include <QObject>
#include <QMutex>
#include <QHash>

#include <memory>

namespace openmittsu {
	namespace dataproviders {

		class KeyRegistry : public QObject {
			Q_OBJECT
		public:
			KeyRegistry(openmittsu::crypto::PublicKey const& serverLongTermPublicKey, openmittsu::database::DatabaseWrapper const& database);
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
			KeyRegistry() = delete;
			void invalidateCache();
			void updateCache();

			mutable QMutex m_mutex;
			bool m_isCacheValid;

			openmittsu::protocol::ContactId m_cachedSelfContactId;
			openmittsu::crypto::KeyPair m_cachedClientLongTermKeyPair;
			openmittsu::crypto::PublicKey const m_serverLongTermPublicKey;

			QHash<openmittsu::protocol::ContactId, openmittsu::database::ContactData> m_cachedPublicKeys;

			openmittsu::database::DatabaseWrapper m_database;
		};

	}
}

#endif // OPENMITTSU_DATAPROVIDERS_KEYREGISTRY_H_
