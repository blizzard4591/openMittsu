#include "src/protocol/GroupId.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/ByteArrayConversions.h"
#include "sodium.h"

#include <QHash>

namespace openmittsu {
	namespace protocol {

		GroupId::GroupId() : owner(0), groupId(0) {
			// Used by the QMetaType system
		}

		GroupId::GroupId(quint64 owner, quint64 groupId) : owner(owner), groupId(groupId) {
			// Intentionally left empty.
		}

		GroupId::GroupId(ContactId const& owner, quint64 groupId) : owner(owner), groupId(groupId) {
			// Intentionally left empty.
		}

		GroupId::GroupId(ContactId const& owner, QString const& hexEncodedGroupId) : owner(owner), groupId(openmittsu::utility::ByteArrayConversions::convert8ByteQByteArrayToQuint64(QByteArray::fromHex(hexEncodedGroupId.toUtf8()))) {
			// Intentionally left empty.
		}

		GroupId::GroupId(GroupId const& other) : owner(other.owner), groupId(other.groupId) {
			// Intentionally left empty.
		}

		GroupId::~GroupId() {
			// Intentionally left empty.
		}

		quint64 GroupId::getGroupId() const {
			return groupId;
		}

		ContactId const& GroupId::getOwner() const {
			return owner;
		}

		QByteArray GroupId::getGroupIdAsByteArray() const {
			QByteArray result;
			result.append(owner.getContactIdAsByteArray());
			result.append(openmittsu::utility::ByteArrayConversions::convertQuint64toQByteArray(groupId));

			return result;
		}

		GroupId GroupId::fromData(QByteArray const& data) {
			if (data.size() < getSizeOfGroupIdInBytes()) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Need at least " << getSizeOfGroupIdInBytes() << " Bytes for creating a Group Id from Data.";
			}

			ContactId const owner(data.mid(0, ContactId::getSizeOfContactIdInBytes()));
			quint64 groupId = openmittsu::utility::ByteArrayConversions::convert8ByteQByteArrayToQuint64(data.mid(ContactId::getSizeOfContactIdInBytes(), PROTO_GROUP_GROUPID_LENGTH_BYTES));

			return GroupId(owner, groupId);
		}

		GroupId GroupId::createRandomGroupId(ContactId const& owner) {
			quint64 result = 0;
			randombytes_buf(&result, sizeof(quint64));

			return GroupId(owner, result);
		}

		bool GroupId::operator ==(GroupId const& other) const {
			return (groupId == other.groupId) && (owner == other.owner);
		}

		bool GroupId::operator !=(GroupId const& other) const {
			return (groupId != other.groupId) || (owner != other.owner);
		}

		bool GroupId::operator <(GroupId const& other) const {
			return (owner < other.owner) || ((owner == other.owner) && (groupId < other.groupId));
		}

		std::string GroupId::toString() const {
			return QString("%1 owned by ID %2").arg(QString(openmittsu::utility::ByteArrayConversions::convertQuint64toQByteArray(groupId).toHex())).arg(owner.toQString()).toStdString();
		}

		QString GroupId::toQString() const {
			return QString("%1 owned by ID %2").arg(QString(openmittsu::utility::ByteArrayConversions::convertQuint64toQByteArray(groupId).toHex())).arg(owner.toQString());
		}

		QString GroupId::groupIdWithoutOwnerToQString() const {
			return QString(openmittsu::utility::ByteArrayConversions::convertQuint64toQByteArray(groupId).toHex());
		}

		QString GroupId::toContactFileFormat() const {
			return QString("%1 : %2").arg(QString(openmittsu::utility::ByteArrayConversions::convertQuint64toQByteArray(groupId).toHex())).arg(owner.toQString());
		}

		uint qHash(GroupId const& key, uint seed) {
			return ::qHash(key.getGroupId(), seed) ^ ::qHash(key.getOwner().getContactId(), seed);
		}
	}
}
