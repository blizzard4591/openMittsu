#ifndef OPENMITTSU_WIDGETS_GROUPLISTWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_GROUPLISTWIDGETITEM_H_

#include <QListWidget>
#include <QListWidgetItem>
#include <QString>
#include <QByteArray>

#include <cstdint>

#include "src/protocol/GroupId.h"

namespace openmittsu {
	namespace widgets {

		class GroupListWidgetItem : public QListWidgetItem {
		public:
			virtual ~GroupListWidgetItem() {}
			GroupListWidgetItem(openmittsu::protocol::GroupId const& group, bool sortById, const QString& text, QListWidget* parent = nullptr, int type = Type);

			openmittsu::protocol::GroupId const& getGroupId() const;

			virtual bool operator<(QListWidgetItem const& other) const override;
		private:
			openmittsu::protocol::GroupId const m_groupId;
			bool const m_sortById;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_GROUPLISTWIDGETITEM_H_
