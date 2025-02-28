#ifndef OPENMITTSU_PROTOCOL_CONTACTID_H_
#define OPENMITTSU_PROTOCOL_CONTACTID_H_

#include <cstdint>
#include <QtGlobal>
#include <QByteArray>
#include <QMetaType>
#include <QString>

#include "src/protocol/ProtocolSpecs.h"

namespace openmittsu {
	namespace protocol {

		class ContactId {
		public:
			ContactId();
			explicit ContactId(quint64 contactId);
			explicit ContactId(QString const& identityString);
			ContactId(QByteArray const& contactIdBytes);
			ContactId(ContactId const& other);
			virtual ~ContactId();

			quint64 getContactId() const;
			QByteArray getContactIdAsByteArray() const;

			static constexpr int getSizeOfContactIdInBytes() { return (PROTO_IDENTITY_LENGTH_BYTES); }
			std::string toString() const;
			QString toQString() const;

			static bool isValidContactId(QString const& identity);

			bool operator ==(ContactId const& other) const;
			bool operator !=(ContactId const& other) const;
			bool operator <(ContactId const& other) const;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
			friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<ContactId, true>;
#endif
		private:
			quint64 contactId;
		};

		uint qHash(ContactId const& key, uint seed);
	}
}

Q_DECLARE_METATYPE(openmittsu::protocol::ContactId)

#endif // OPENMITTSU_PROTOCOL_CONTACTID_H_
