#ifndef OPENMITTSU_UTILITY_LEGACYCONTACTIMPORTER_H_
#define OPENMITTSU_UTILITY_LEGACYCONTACTIMPORTER_H_

#include <QHash>
#include <QObject>
#include <QString>

#include <cstdint>
#include <utility>

#include "src/crypto/PublicKey.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"

namespace openmittsu {
	namespace utility {

		class LegacyContactImporter : public QObject {
			Q_OBJECT
		public:
			LegacyContactImporter(LegacyContactImporter const& other);
			virtual ~LegacyContactImporter();
			static LegacyContactImporter fromFile(QString const& filename);

			int getContactCount() const;
			int getGroupCount() const;

			QHash<openmittsu::protocol::ContactId, std::pair<openmittsu::crypto::PublicKey, QString>> const& getContacts() const;
			QHash<openmittsu::protocol::GroupId, std::pair<QSet<openmittsu::protocol::ContactId>, QString>> const& getGroups() const;
		private:
			LegacyContactImporter(QHash<openmittsu::protocol::ContactId, std::pair<openmittsu::crypto::PublicKey, QString>> const& contacts, QHash<openmittsu::protocol::GroupId, std::pair<QSet<openmittsu::protocol::ContactId>, QString>> const& groups);

			QHash<openmittsu::protocol::ContactId, std::pair<openmittsu::crypto::PublicKey, QString>> const m_contacts;
			QHash<openmittsu::protocol::GroupId, std::pair<QSet<openmittsu::protocol::ContactId>, QString>> const m_groups;
		};

	}
}

#endif // OPENMITTSU_UTILITY_LEGACYCONTACTIMPORTER_H_