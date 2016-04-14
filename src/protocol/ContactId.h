#ifndef OPENMITTSU_PROTOCOL_CONTACTID_H_
#define OPENMITTSU_PROTOCOL_CONTACTID_H_

#include "protocol/Id.h"
#include <cstdint>
#include <QtGlobal>
#include <QByteArray>
#include <QMetaType>
#include <QString>

class ContactId : public Id {
public:
	explicit ContactId(quint64 contactId);
	explicit ContactId(QString const& identityString);
	ContactId(QByteArray const& contactIdBytes);
	ContactId(ContactId const& other);
	virtual ~ContactId();

	quint64 getContactId() const;
	QByteArray getContactIdAsByteArray() const;

	static int getSizeOfContactIdInBytes();
	virtual std::string toString() const override;
	virtual QString toQString() const override;

	bool operator ==(ContactId const& other) const;
	bool operator <(ContactId const& other) const;

	friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<ContactId, true>;
private:
	quint64 contactId;

	ContactId();
};

uint qHash(ContactId const& key, uint seed);

Q_DECLARE_METATYPE(ContactId)

#endif // OPENMITTSU_PROTOCOL_CONTACTID_H_
