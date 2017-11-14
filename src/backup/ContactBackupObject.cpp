#include "src/backup/ContactBackupObject.h"

#include "src/exceptions/IllegalArgumentException.h"

namespace openmittsu {
	namespace backup {

		ContactBackupObject::ContactBackupObject(openmittsu::protocol::ContactId const& id, openmittsu::crypto::PublicKey const& publicKey, openmittsu::protocol::ContactIdVerificationStatus const& verificationStatus, QString const& firstName, QString const& lastName, QString const& nickName, int const& color) : BackupObject(), 
			m_id(id), m_publicKey(publicKey), m_verificationStatus(verificationStatus), m_firstName(firstName), m_lastName(lastName), m_nickName(nickName), m_color(color) {
			//
		}
		
		ContactBackupObject::~ContactBackupObject() {
			//
		}

		openmittsu::protocol::ContactId const& ContactBackupObject::getContactId() const {
			return m_id;
		}
		
		openmittsu::crypto::PublicKey const& ContactBackupObject::getPublicKey() const {
			return m_publicKey;
		}
		
		openmittsu::protocol::ContactIdVerificationStatus const ContactBackupObject::getVerificationStatus() const {
			return m_verificationStatus;
		}
		
		QString const& ContactBackupObject::getFirstName() const {
			return m_firstName;
		}
		
		QString const& ContactBackupObject::getLastName() const {
			return m_lastName;
		}
		
		QString const& ContactBackupObject::getNickName() const {
			return m_nickName;
		}
		
		int ContactBackupObject::getColor() const {
			return m_color;
		}

		ContactBackupObject ContactBackupObject::fromBackupMatch(QString const&, QHash<QString, int> const& headerOffsets, SimpleCsvLineSplitter const& splittedLines) {
			QSet<QString> const requiredFields = { QStringLiteral("identity"), QStringLiteral("publickey"), QStringLiteral("verification"), QStringLiteral("firstname"), QStringLiteral("lastname"), QStringLiteral("nick_name"), QStringLiteral("color") };
			if (!hasRequiredFields(requiredFields, headerOffsets)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Could not parse data to Contact, not all required fields are present!";
			} else {
				openmittsu::protocol::ContactId const id(openmittsu::protocol::ContactId(splittedLines.getColumn(headerOffsets.value(QStringLiteral("identity")))));
				openmittsu::crypto::PublicKey const publicKey(openmittsu::crypto::PublicKey::fromHexString(splittedLines.getColumn(headerOffsets.value(QStringLiteral("publickey")))));
				openmittsu::protocol::ContactIdVerificationStatus const verificationStatus(openmittsu::protocol::ContactIdVerificationStatusHelper::fromQString(splittedLines.getColumn(headerOffsets.value(QStringLiteral("verification")))));
				QString const firstName(splittedLines.getColumn(headerOffsets.value(QStringLiteral("firstname"))));
				QString const lastName(splittedLines.getColumn(headerOffsets.value(QStringLiteral("lastname"))));
				QString const nickName(splittedLines.getColumn(headerOffsets.value(QStringLiteral("nick_name"))));
				int const color(splittedLines.getColumn(headerOffsets.value(QStringLiteral("color"))).toInt());

				return ContactBackupObject(id, publicKey, verificationStatus, firstName, lastName, nickName, color);
			}
		}

	}
}
