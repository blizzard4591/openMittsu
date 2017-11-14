#include "src/protocol/ContactIdList.h"

#include "src/utility/Endian.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/ByteArrayConversions.h"

#include <QHash>

namespace openmittsu {
	namespace protocol {

		ContactIdList::ContactIdList(QSet<ContactId> const& contactIds) : m_contactIds(contactIds) {
			//
		}

		ContactIdList::~ContactIdList() {
			//
		}

		QSet<ContactId> const& ContactIdList::getContactIds() const {
			return m_contactIds;
		}

		QString ContactIdList::toString(QChar const& separator) const {
			QString result = "";
			result.reserve(m_contactIds.size() * (ContactId::getSizeOfContactIdInBytes() + 1));

			auto it = m_contactIds.constBegin();
			auto const end = m_contactIds.constEnd();
			for (; it != end; ++it) {
				if (!result.isEmpty()) {
					result.append(separator);
				}
				result.append(it->toQString());
			}

			return result;
		}

		ContactIdList ContactIdList::fromString(QString const& contactIdsString, QChar const& separator) {
			QSet<ContactId> result;
			QStringList const splitMembers = contactIdsString.split(separator, QString::SkipEmptyParts);
			for (int i = 0; i < splitMembers.size(); ++i) {
				openmittsu::protocol::ContactId const memberId(splitMembers.at(i));
				result.insert(memberId);
			}
			return ContactIdList(result);
		}

	}
}
