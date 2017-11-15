#include "src/dataproviders/KeyRegistry.h"

#include "src/database/Database.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"

#include <QMutexLocker>

namespace openmittsu {
	namespace dataproviders {

		KeyRegistry::KeyRegistry() : QObject(), m_mutex(), m_isCacheValid(false), m_cachedSelfContactId(0), m_serverLongTermPublicKey(), m_database() {
			throw;
		}

		KeyRegistry::KeyRegistry(KeyRegistry const& other) : m_mutex(), m_isCacheValid(false), m_cachedSelfContactId(0), m_serverLongTermPublicKey(other.m_serverLongTermPublicKey), m_database(other.m_database) {
			{
				auto db = m_database.lock();
				if (!db) {
					throw openmittsu::exceptions::InternalErrorException() << "KeyRegistry::KeyRegistry() called while the database is unavailable.";
				} else {
					OPENMITTSU_CONNECT(db.get(), contactChanged(openmittsu::protocol::ContactId const&), this, onContactChanged());
				}
			}
			onContactChanged();
		}

		KeyRegistry::KeyRegistry(openmittsu::crypto::PublicKey const& serverLongTermPublicKey, std::weak_ptr<openmittsu::database::Database> const& database)
			: m_mutex(), m_isCacheValid(false), m_cachedSelfContactId(0), m_serverLongTermPublicKey(serverLongTermPublicKey), m_database(database) {
			{
				auto db = m_database.lock();
				if (!db) {
					throw openmittsu::exceptions::InternalErrorException() << "KeyRegistry::KeyRegistry() called while the database is unavailable.";
				} else {
					OPENMITTSU_CONNECT(db.get(), contactChanged(openmittsu::protocol::ContactId const&), this, onContactChanged());
				}
			}
			onContactChanged();
		}

		KeyRegistry::~KeyRegistry() {
			// Intentionally left empty.
		}

		void KeyRegistry::onContactChanged() {
			QMutexLocker mutexLock(&m_mutex);
			invalidateCache();
			updateCache();
		}

		openmittsu::protocol::ContactId KeyRegistry::getSelfContactId() const {
			QMutexLocker mutexLock(&m_mutex);
			if (!m_isCacheValid) {
				throw openmittsu::exceptions::InternalErrorException() << "KeyRegistry::getSelfContactId() called while the cache is invalid.";
			}

			return m_cachedSelfContactId;
		}

		bool KeyRegistry::hasIdentity(openmittsu::protocol::ContactId const& identity) const {
			QMutexLocker mutexLock(&m_mutex);
			if (!m_isCacheValid) {
				throw openmittsu::exceptions::InternalErrorException() << "KeyRegistry::hasIdentity(identity = " << identity.toString() << ") called while the cache is invalid.";
			}

			return m_cachedPublicKeys.contains(identity);
		}

		openmittsu::crypto::PublicKey KeyRegistry::getPublicKeyForIdentity(openmittsu::protocol::ContactId const& identity) const {
			QMutexLocker mutexLock(&m_mutex);
			if (!m_isCacheValid) {
				throw openmittsu::exceptions::InternalErrorException() << "KeyRegistry::getPublicKeyForIdentity(identity = " << identity.toString() << ") called while the cache is invalid.";
			} else if (!m_cachedPublicKeys.contains(identity)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "KeyRegistry::getPublicKeyForIdentity(identity = " << identity.toString() << ") called with identity that does not exist.";
			} else {
				return m_cachedPublicKeys.value(identity);
			}
		}

		openmittsu::crypto::KeyPair const& KeyRegistry::getClientLongTermKeyPair() const {
			QMutexLocker mutexLock(&m_mutex);
			if (!m_isCacheValid) {
				throw openmittsu::exceptions::InternalErrorException() << "KeyRegistry::getClientLongTermKeyPair() called while the cache is not prepared.";
			}

			return m_cachedClientLongTermKeyPair;
		}

		openmittsu::crypto::PublicKey const& KeyRegistry::getServerLongTermPublicKey() const {
			return m_serverLongTermPublicKey;
		}

		void KeyRegistry::invalidateCache() {
			this->m_isCacheValid = false;
		}
		
		void KeyRegistry::updateCache() {
			auto db = m_database.lock();
			if (!db) {
				throw openmittsu::exceptions::InternalErrorException() << "KeyRegistry::updateCache() called while the database is unavailable.";
			} else {
				openmittsu::backup::IdentityBackup const backupData = db->getBackup();

				m_cachedSelfContactId = backupData.getClientContactId();
				m_cachedClientLongTermKeyPair = backupData.getClientLongTermKeyPair();
				m_cachedPublicKeys = db->getKnownContactsWithPublicKeys();

				this->m_isCacheValid = true;
				LOGGER_DEBUG("Updated cache in KeyRegistry.");
			}
		}
	}
}
