#ifndef OPENMITTSU_PROTOCOL_GROUPID_H_
#define OPENMITTSU_PROTOCOL_GROUPID_H_

#include "protocol/Id.h"
#include "protocol/ContactId.h"
#include "protocol/MessageId.h"
#include <cstdint>
#include <QtGlobal>
#include <QByteArray>
#include <QMetaType>
#include <QHash>

class GroupId : public Id {
public:
	GroupId(ContactId const& owner, quint64 groupId);
	explicit GroupId(quint64 owner, quint64 groupId);
	GroupId(GroupId const& other);
	virtual ~GroupId();

	ContactId const& getOwner() const;
	quint64 getGroupId() const;
	QByteArray getGroupIdAsByteArray() const;
	static int getSizeOfGroupIdInBytes();
	virtual std::string toString() const override;
	virtual QString toQString() const override;

	QString toContactFileFormat() const;

	bool operator ==(GroupId const& other) const;
	bool operator <(GroupId const& other) const;

	static GroupId fromData(QByteArray const& data);
	static GroupId createRandomGroupId(ContactId const& owner);

	friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<GroupId, true>;
	friend class QHash<MessageId, GroupId>;
private:
	ContactId owner;
	quint64 groupId;

	GroupId();
};

uint qHash(GroupId const& key, uint seed);

Q_DECLARE_METATYPE(GroupId)

#endif // OPENMITTSU_PROTOCOL_GROUPID_H_
