#ifndef OPENMITTSU_WIDGETS_SIMPLETABCONTROLLER_H_
#define OPENMITTSU_WIDGETS_SIMPLETABCONTROLLER_H_

#include "src/widgets/TabController.h"

#include <QHash>

namespace openmittsu {
	namespace widgets {
		class ChatTab;
		class ChatTabWidget;

		class SimpleTabController : public openmittsu::widgets::TabController {
			Q_OBJECT
		public:
			SimpleTabController(ChatTabWidget* chatTabWidget);
			virtual ~SimpleTabController();

			virtual bool hasTab(openmittsu::protocol::ContactId const& contact) const override;
			virtual bool hasTab(openmittsu::protocol::GroupId const& group) const override;

			virtual QWidget* getTab(openmittsu::protocol::ContactId const& contact) const override;
			virtual QWidget* getTab(openmittsu::protocol::GroupId const& group) const override;

			virtual void openTab(openmittsu::protocol::ContactId const& contact, openmittsu::dataproviders::BackedContact const& backedContact) override;
			virtual void openTab(openmittsu::protocol::GroupId const& group, openmittsu::dataproviders::BackedGroup const& backedGroup) override;

			virtual void closeTab(openmittsu::protocol::ContactId const& contact) override;
			virtual void closeTab(openmittsu::protocol::GroupId const& group) override;

			virtual void focusTab(openmittsu::protocol::ContactId const& contact) override;
			virtual void focusTab(openmittsu::protocol::GroupId const& group) override;
		public slots:
			void slotTabCloseRequested(int index);
		private:
			ChatTabWidget * const m_chatTabWidget;

			QHash<openmittsu::protocol::ContactId, ChatTab*> m_contactTabs;
			QHash<openmittsu::protocol::GroupId, ChatTab*> m_groupTabs;
		};
	}
}

#endif // OPENMITTSU_WIDGETS_SIMPLETABCONTROLLER_H_
