#ifndef OPENMITTSU_CONTACT_H_
#define OPENMITTSU_CONTACT_H_

#include <QObject>
#include <cstdint>

#include "protocol/ContactId.h"

class QString;

class Contact : public QObject {
	Q_OBJECT
public:
	enum class ContactType {
		CONTACT_IDENTITY,
		CONTACT_GROUP
	};

	Contact();
	virtual ~Contact() {}

	virtual ContactType getContactType() const = 0;
	virtual QString getContactName() const = 0;
	virtual QString getContactDescription() const = 0;
signals:
	
	
	// Signals for the ContactRegistry
	void dataChanged();
};

#endif // OPENMITTSU_CONTACT_H_