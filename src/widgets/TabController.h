#ifndef OPENMITTSU_WIDGETS_TABCONTROLLER_H_
#define OPENMITTSU_WIDGETS_TABCONTROLLER_H_

#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"

#include <QObject>
#include <QWidget>

namespace openmittsu {
	namespace dataproviders {
		class BackedContact;
		class BackedGroup;
	}

	namespace widgets {
		class TabController : public QObject {
			Q_OBJECT
		public:
			virtual ~TabController() {}

			virtual bool hasTab(openmittsu::protocol::ContactId const& contact) const = 0;
			virtual bool hasTab(openmittsu::protocol::GroupId const& group) const = 0;

			virtual QWidget* getTab(openmittsu::protocol::ContactId const& contact) const = 0;
			virtual QWidget* getTab(openmittsu::protocol::GroupId const& group) const = 0;

			virtual void openTab(openmittsu::protocol::ContactId const& contact, openmittsu::dataproviders::BackedContact const& backedContact) = 0;
			virtual void openTab(openmittsu::protocol::GroupId const& group, openmittsu::dataproviders::BackedGroup const& backedGroup) = 0;

			virtual void closeTab(openmittsu::protocol::ContactId const& contact) = 0;
			virtual void closeTab(openmittsu::protocol::GroupId const& group) = 0;

			virtual void focusTab(openmittsu::protocol::ContactId const& contact) = 0;
			virtual void focusTab(openmittsu::protocol::GroupId const& group) = 0;
		};
	}
}

#endif // OPENMITTSU_WIDGETS_TABCONTROLLER_H_
