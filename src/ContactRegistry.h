#ifndef OPENMITTSU_CONTACTREGISTRY_H_
#define OPENMITTSU_CONTACTREGISTRY_H_

#include <QObject>
#include <QString>
#include <QHash>
#include <QByteArray>
#include <QMutex>

#include <cstdint>
#include <utility>

#include "IdentityContact.h"
#include "GroupContact.h"

#include "protocol/ContactId.h"
#include "protocol/GroupId.h"

class ContactRegistry : public QObject {
	Q_OBJECT
public:
	static ContactRegistry* getInstance();

	void fromFile(QString const& filename, bool dryRun = false);
	void toFile(QString const& filename) const;

	bool hasIdentity(ContactId const& identity) const;
	bool hasGroup(GroupId const& identity) const;

	IdentityContact* getSelfContact() const;
	void setSelfContact(IdentityContact* newSelfContact);

	PublicKey const& getPublicKey(ContactId const& identity) const;
	QString getNickname(ContactId const& identity) const;
	QString getNickname(GroupId const& group) const;
	int getIdentityCount() const;
	int getGroupCount() const;

	void addIdentity(IdentityContact* contact);
	void addGroup(GroupContact* contact);
	IdentityContact* getIdentity(ContactId const& identity) const;
	GroupContact* getGroup(GroupId const& identity) const;

	QList<ContactId> getIdentities() const;
	QList<GroupId> getGroups() const;

	QHash<ContactId, PublicKey> getKnownIdentitiesWithPublicKeys() const;
	QHash<GroupId, std::pair<QSet<ContactId>, QString>> getKnownGroupsWithMembersAndTitles() const;
signals:
	void identitiesChanged();
public slots:
	void onContactDataChanged();
private:
	ContactRegistry();

	ContactRegistry(const ContactRegistry &); // hide copy constructor
	ContactRegistry& operator=(const ContactRegistry &); // hide assign op
	// we leave just the declarations, so the compiler will warn us 
	// if we try to use those two functions by accident

	static ContactRegistry* instance;

	mutable QMutex accessMutex;
	QHash<ContactId, IdentityContact*> identityToIdentityContactHashMap;
	QHash<GroupId, GroupContact*> identityToGroupContactHashMap;

	// default, empty string
	QString const emptyString;

	IdentityContact* selfContact;

	void connectContact(Contact* contact);
	void disconnectContact(Contact* contact);
};

#endif // OPENMITTSU_CONTACTREGISTRY_H_
