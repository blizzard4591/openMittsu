#include "src/utility/LegacyContactImporter.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"

#include <QFile>
#include <QSet>
#include <QTextStream>
#include <QRegularExpression>

namespace openmittsu {
	namespace utility {

		LegacyContactImporter::LegacyContactImporter(QHash<openmittsu::protocol::ContactId, std::pair<openmittsu::crypto::PublicKey, QString>> const& contacts, QHash<openmittsu::protocol::GroupId, std::pair<QSet<openmittsu::protocol::ContactId>, QString>> const& groups)
			: QObject(), m_contacts(contacts), m_groups(groups) {
			//
		}

		LegacyContactImporter::LegacyContactImporter(LegacyContactImporter const& other) : QObject(), m_contacts(other.m_contacts), m_groups(other.m_groups) {
			//
		}

		LegacyContactImporter::~LegacyContactImporter() {
			//
		}
		
		int LegacyContactImporter::getContactCount() const {
			return m_contacts.size();
		}

		int LegacyContactImporter::getGroupCount() const {
			return m_groups.size();
		}

		LegacyContactImporter LegacyContactImporter::fromFile(QString const& filename) {
			QHash<openmittsu::protocol::ContactId, std::pair<openmittsu::crypto::PublicKey, QString>> contacts;
			QHash<openmittsu::protocol::GroupId, std::pair<QSet<openmittsu::protocol::ContactId>, QString>> groups;

			if (!QFile::exists(filename)) {
				throw openmittsu::exceptions::IllegalArgumentException() << QString("Could not open the specified contacts file as it does not exist: %1").arg(filename).toStdString();
			}

			QFile inputFile(filename);
			if (!inputFile.open(QFile::ReadOnly | QFile::Text)) {
				throw openmittsu::exceptions::IllegalArgumentException() << QString("Could not open the specified contacts file for reading: %1").arg(filename).toStdString();
			}

			QRegularExpression commentRegExp("^\\s*#.*$", QRegularExpression::CaseInsensitiveOption);
			QRegularExpression identityRegExp("^\\s*([A-Z0-9*][A-Z0-9]{7})\\s*:\\s*([a-fA-F0-9]{64})\\s*(?::\\s*(.*)\\s*)?$", QRegularExpression::CaseInsensitiveOption);
			QRegularExpression groupRegExp("^\\s*([a-fA-F0-9]{16})\\s*:\\s*([A-Z0-9*][A-Z0-9]{7})\\s*:\\s*([A-Z0-9*][A-Z0-9]{7}(?:\\s*,\\s*[A-Z0-9*][A-Z0-9]{7})*)\\s*:\\s*(.*)\\s*$", QRegularExpression::CaseInsensitiveOption);

			QTextStream in(&inputFile);
#if defined(QT_VERSION) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
			in.setCodec("UTF-8"); // change the file codec to UTF-8.
#endif
			while (!in.atEnd()) {
				QString line = in.readLine().trimmed();
				if (line.isEmpty() || commentRegExp.match(line).hasMatch()) {
					continue;
				}
				auto const identityRegExpMatch = identityRegExp.match(line);
				auto const groupRegExpMatch = groupRegExp.match(line);
				if (identityRegExpMatch.hasMatch()) {
					openmittsu::protocol::ContactId const contactId(identityRegExpMatch.captured(1));
					openmittsu::crypto::PublicKey const publicKey = openmittsu::crypto::PublicKey::fromHexString(identityRegExpMatch.captured(2));
					QString const trimmedNickname = identityRegExpMatch.captured(3).trimmed();
					contacts.insert(contactId, std::make_pair(publicKey, trimmedNickname));
				} else if (groupRegExpMatch.hasMatch()) {
					openmittsu::protocol::ContactId const groupOwner(groupRegExpMatch.captured(2));
					QString const groupIdHexString(groupRegExpMatch.captured(1));
					openmittsu::protocol::GroupId const groupId(groupOwner, groupIdHexString);
#if defined(QT_VERSION) && (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
					QStringList ids = groupRegExpMatch.captured(3).split(',', Qt::SkipEmptyParts);
#else
					QStringList ids = groupRegExpMatch.captured(3).split(',', QString::SkipEmptyParts);
#endif
					QSet<openmittsu::protocol::ContactId> groupMembers;
					QStringList::const_iterator it = ids.constBegin();
					QStringList::const_iterator end = ids.constEnd();
					for (; it != end; ++it) {
						QString trimmedId(it->trimmed());
						openmittsu::protocol::ContactId const memberId(trimmedId);
						groupMembers.insert(memberId);
					}
					QString const groupName = groupRegExpMatch.captured(4).trimmed();

					groups.insert(groupId, std::make_pair(groupMembers, groupName));
				}
			}

			inputFile.close();

			return LegacyContactImporter(contacts, groups);
		}

		QHash<openmittsu::protocol::ContactId, std::pair<openmittsu::crypto::PublicKey, QString>> const& LegacyContactImporter::getContacts() const {
			return m_contacts;
		}

		QHash<openmittsu::protocol::GroupId, std::pair<QSet<openmittsu::protocol::ContactId>, QString>> const& LegacyContactImporter::getGroups() const {
			return m_groups;
		}

	}
}
