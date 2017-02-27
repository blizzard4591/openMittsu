#ifndef OPENMITTSU_IDENTITYCONTACT_H_
#define OPENMITTSU_IDENTITYCONTACT_H_

#include <QObject>
#include <QString>
#include <QByteArray>
#include <cstdint>

#include "Contact.h"
#include "PublicKey.h"
#include "protocol/ContactId.h"
#include "protocol/ContactIdStatus.h"

class IdentityContact : public Contact {
	Q_OBJECT
public:
	IdentityContact(ContactId const& contactIdentity, PublicKey const& contactPublicKey);
	virtual ~IdentityContact();

	virtual ContactType getContactType() const override;
	virtual QString getContactName() const override;
	virtual QString getContactDescription() const override;
	void setNickname(QString const& nickname);
	QString const& getNickname() const;
	PublicKey const& getPublicKey() const;
	ContactId const& getContactId() const;
	ContactIdStatus const& getStatus() const;
	void setStatus(ContactIdStatus const& newStatus);
private:
	ContactId const contactId;
	PublicKey const contactPublicKey;

	ContactIdStatus status;
	QString nickname;
};

#endif // OPENMITTSU_IDENTITYCONTACT_H_