#ifndef OPENMITTSU_DATAPROVIDERS_BACKEDGROUP_H_
#define OPENMITTSU_DATAPROVIDERS_BACKEDGROUP_H_

#include <QObject>
#include <QString>
#include <QSet>
#include <QVector>
#include <memory>

#include "src/utility/Location.h"
#include "src/dataproviders/ContactDataProvider.h"
#include "src/dataproviders/GroupDataProvider.h"
#include "src/dataproviders/MessageSource.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"

#include "src/dataproviders/messages/GroupMessageCursor.h"

namespace openmittsu {
	namespace database {
		class Database;
	}

	namespace dataproviders {
		class BackedGroupMessage;
		class MessageCenter;

		class BackedGroup : public QObject, public MessageSource {
			Q_OBJECT
		public:
			BackedGroup(openmittsu::protocol::GroupId const& groupId, GroupDataProvider& dataProvider, ContactDataProvider& contactDataProvider, openmittsu::dataproviders::MessageCenter& messageCenter);
			BackedGroup(BackedGroup const& other);
			virtual ~BackedGroup();

			QString getTitle() const;
			QString getDescription() const;

			virtual int getMessageCount() const override;

			bool hasImage() const;
			QByteArray getImage() const;

			bool hasMember(openmittsu::protocol::ContactId const& identity) const;

			openmittsu::protocol::GroupId const& getId() const;
			QSet<openmittsu::protocol::ContactId> getMembers() const;

			void setGroupTitle(QString const& newTitle);
			void setGroupImage(QByteArray const& newImage);
			void setGroupMembers(QSet<openmittsu::protocol::ContactId> const& newMembers);

			virtual QVector<QString> getLastMessageUuids(std::size_t n) override;
			BackedGroupMessage getMessageByUuid(QString const& uuid);
		public slots:
			bool sendTextMessage(QString const& text);
			bool sendImageMessage(QByteArray const& image, QString const& caption);
			bool sendLocationMessage(openmittsu::utility::Location const& location);
		signals:
			void groupDataChanged();
			void newMessageAvailable(QString const& uuid);
		private:
			openmittsu::protocol::GroupId const m_groupId;
			GroupDataProvider& m_dataProvider;
			ContactDataProvider& m_contactDataProvider;
			openmittsu::dataproviders::MessageCenter& m_messageCenter;

			std::shared_ptr<messages::GroupMessageCursor> m_cursor;
		private slots:
			void slotGroupChanged(openmittsu::protocol::GroupId const& changedGroupId);
			void slotIdentityChanged(openmittsu::protocol::ContactId const& changedContactId);
			void slotNewMessage(openmittsu::protocol::GroupId const& group, QString const& messageUuid);
		};

	}
}

#endif // OPENMITTSU_DATAPROVIDERS_BACKEDGROUP_H_
