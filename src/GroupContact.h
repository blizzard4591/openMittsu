#ifndef OPENMITTSU_GROUPCONTACT_H_
#define OPENMITTSU_GROUPCONTACT_H_

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QSet>
#include <cstdint>

#include "Contact.h"
#include "protocol/ContactId.h"
#include "protocol/GroupId.h"

class GroupContact : public Contact {
	Q_OBJECT
public:
	GroupContact(GroupId const& groupId);
	virtual ~GroupContact();

	virtual ContactType getContactType() const override;
	virtual QString getContactName() const override;
	virtual QString getContactDescription() const override;

	bool hasMember(ContactId const& identity) const;
	void addMember(ContactId const& identity);
	void addMember(QSet<ContactId> const& newMemberSet);
	void updateMembers(QSet<ContactId> const& newMemberSet);
	void removeMember(ContactId const& identity);
	void setGroupName(QString const& groupName);
	GroupId const& getGroupId() const;
	ContactId const& getGroupOwner() const;
	QSet<ContactId> const& getGroupMembers() const;

	static quint64 generateRandomGroupId();
private:
	GroupId const groupId;

	QString groupName;
	QSet<ContactId> groupMembers;
	quint64 messageIdWasReceived;
};

#endif // OPENMITTSU_GROUPCONTACT_H_