#ifndef OPENMITTSU_PROTOCOL_PUSHFROMID_H_
#define OPENMITTSU_PROTOCOL_PUSHFROMID_H_

#include "protocol/Id.h"
#include <QByteArray>

class ContactId;

class PushFromId : public Id {
public:
	PushFromId(ContactId const& contactId);
	PushFromId(QByteArray const& pushFromId);
	PushFromId(PushFromId const& other);
	virtual ~PushFromId();

	bool containsStandardContactId() const;
	quint64 getPushFromId() const;
	QByteArray getPushFromIdAsByteArray() const;
	static int getSizeOfPushFromIdInBytes();
	virtual std::string toString() const override;
	virtual QString toQString() const override;

	bool operator ==(PushFromId const& other) const;
private:
	QByteArray const pushFromIdBytes;

	static QByteArray expandFromContactId(ContactId const& contactId);

	// Disable the default constructor
	PushFromId();
};

#endif // OPENMITTSU_PROTOCOL_PUSHFROMID_H_
