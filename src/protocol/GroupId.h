#ifndef OPENMITTSU_PROTOCOL_GROUPID_H_
#define OPENMITTSU_PROTOCOL_GROUPID_H_

#include "src/protocol/ContactId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/QtVersions.h"

#include <cstdint>
#include <QtGlobal>
#include <QByteArray>
#include <QMetaType>
#include <QHash>

namespace openmittsu {
	namespace protocol {

		class GroupId {
		public:
			GroupId(ContactId const& owner, quint64 groupId);
			explicit GroupId(quint64 owner, quint64 groupId);
			explicit GroupId(ContactId const& owner, QString const& hexEncodedGroupId);
			GroupId(GroupId const& other);
			virtual ~GroupId();

			ContactId const& getOwner() const;
			quint64 getGroupId() const;
			QByteArray getGroupIdAsByteArray() const;
			static constexpr int getSizeOfGroupIdInBytes() { return ((PROTO_GROUP_GROUPID_LENGTH_BYTES) + (PROTO_IDENTITY_LENGTH_BYTES)); }
			std::string toString() const;
			QString toQString() const;
			QString groupIdWithoutOwnerToQString() const;

			QString toContactFileFormat() const;

			bool operator ==(GroupId const& other) const;
			bool operator !=(GroupId const& other) const;
			bool operator <(GroupId const& other) const;

			static GroupId fromData(QByteArray const& data);
			static GroupId createRandomGroupId(ContactId const& owner);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
			friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<GroupId, true>;
#endif
			friend class QHash<MessageId, GroupId>;
		private:
			ContactId owner;
			quint64 groupId;

			GroupId();
		};


		utility::QtHashSizeType qHash(GroupId const& key, utility::QtHashSizeType seed);
	}
}

Q_DECLARE_METATYPE(openmittsu::protocol::GroupId)

#endif // OPENMITTSU_PROTOCOL_GROUPID_H_
