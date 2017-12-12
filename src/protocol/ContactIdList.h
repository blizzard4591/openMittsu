#ifndef OPENMITTSU_PROTOCOL_CONTACTIDLIST_H_
#define OPENMITTSU_PROTOCOL_CONTACTIDLIST_H_

#include <QChar>
#include <QSet>
#include <QString>

#include "src/protocol/ContactId.h"

namespace openmittsu {
	namespace protocol {

		class ContactIdList {
		public:
			explicit ContactIdList(QSet<ContactId> const& contactIds);
			virtual ~ContactIdList();

			QSet<ContactId> const& getContactIds() const;
			QString toString(QChar const& separator = QChar(';')) const;
			QString toStringS(QString const& separator = QString(", ")) const;

			static ContactIdList fromString(QString const& contactIdsString, QChar const& separator = QChar(';'));

			friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<ContactIdList, true>;
		private:
			QSet<ContactId> const m_contactIds;

			ContactIdList();
		};

	}
}

Q_DECLARE_METATYPE(openmittsu::protocol::ContactIdList)

#endif // OPENMITTSU_PROTOCOL_CONTACTIDLIST_H_
