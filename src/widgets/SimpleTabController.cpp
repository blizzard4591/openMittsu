#include "src/widgets/SimpleTabController.h"

#include "src/exceptions/InternalErrorException.h"

#include "src/widgets/chat/ChatTabWidget.h"
#include "src/widgets/SimpleContactChatTab.h"
#include "src/widgets/SimpleGroupChatTab.h"

#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace widgets {

		SimpleTabController::SimpleTabController(ChatTabWidget* chatTabWidget) : TabController(), m_chatTabWidget(chatTabWidget) {
			if (chatTabWidget == nullptr) {
				throw openmittsu::exceptions::InternalErrorException() << "ChatTabWidget may not be null.";
			}
			OPENMITTSU_CONNECT(m_chatTabWidget, tabCloseRequested(int), this, slotTabCloseRequested(int));
		}

		SimpleTabController::~SimpleTabController() {
			//
		}

		bool SimpleTabController::hasTab(openmittsu::protocol::ContactId const& contact) const {
			return m_contactTabs.contains(contact);
		}

		bool SimpleTabController::hasTab(openmittsu::protocol::GroupId const& group) const {
			return m_groupTabs.contains(group);
		}

		QWidget* SimpleTabController::getTab(openmittsu::protocol::ContactId const& contact) const {
			return m_contactTabs.value(contact);
		}

		QWidget* SimpleTabController::getTab(openmittsu::protocol::GroupId const& group) const {
			return m_groupTabs.value(group);
		}

		void SimpleTabController::openTab(openmittsu::protocol::ContactId const& contact, openmittsu::dataproviders::BackedContact const& backedContact) {
			if (!hasTab(contact)) {
				ChatTab* chatWindow = new SimpleContactChatTab(backedContact, m_chatTabWidget);
				m_chatTabWidget->addChatTab(chatWindow);
				m_contactTabs.insert(contact, chatWindow);
				LOGGER_DEBUG("Opening ChatTab for contact {}.", contact.toString());
			} else {
				LOGGER_DEBUG("Not opening ChatTab for contact {}, it is already open.", contact.toString());
			}
		}

		void SimpleTabController::openTab(openmittsu::protocol::GroupId const& group, openmittsu::dataproviders::BackedGroup const& backedGroup) {
			if (!hasTab(group)) {
				ChatTab* chatWindow = new SimpleGroupChatTab(backedGroup, m_chatTabWidget);
				m_chatTabWidget->addChatTab(chatWindow);
				m_groupTabs.insert(group, chatWindow);
				LOGGER_DEBUG("Opening ChatTab for group {}.", group.toString());
			} else {
				LOGGER_DEBUG("Not opening ChatTab for group {}, it is already open.", group.toString());
			}
		}

		void SimpleTabController::closeTab(openmittsu::protocol::ContactId const& contact) {
			if (hasTab(contact)) {
				ChatTab* const chatTab = m_contactTabs.value(contact);
				int const index = m_chatTabWidget->indexOf(chatTab);
				if (index == -1) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not look up index of tab for contact " << contact.toString() << "!";
				}
				m_chatTabWidget->removeTab(index);
				m_contactTabs.remove(contact);
				delete chatTab;
				LOGGER_DEBUG("Closing ChatTab for contact {}.", contact.toString());
			} else {
				LOGGER_DEBUG("Not closing ChatTab for contact {}, not open.", contact.toString());
			}
		}

		void SimpleTabController::closeTab(openmittsu::protocol::GroupId const& group) {
			if (hasTab(group)) {
				ChatTab* const chatTab = m_groupTabs.value(group);
				int const index = m_chatTabWidget->indexOf(chatTab);
				if (index == -1) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not look up index of tab for group " << group.toString() << "!";
				}
				m_chatTabWidget->removeTab(index);
				m_groupTabs.remove(group);
				delete chatTab;
				LOGGER_DEBUG("Closing ChatTab for group {}.", group.toString());
			} else {
				LOGGER_DEBUG("Not closing ChatTab for group {}, not open.", group.toString());
			}
		}

		void SimpleTabController::focusTab(openmittsu::protocol::ContactId const& contact) {
			if (hasTab(contact)) {
				ChatTab* const chatTab = m_contactTabs.value(contact);
				m_chatTabWidget->setCurrentWidget(chatTab);
				LOGGER_DEBUG("Changing focus on ChatTab for contact {}.", contact.toString());
			} else {
				LOGGER_DEBUG("Not changing focus on ChatTab for contact {}, not open.", contact.toString());
			}
		}

		void SimpleTabController::focusTab(openmittsu::protocol::GroupId const& group) {
			if (hasTab(group)) {
				ChatTab* const chatTab = m_groupTabs.value(group);
				m_chatTabWidget->setCurrentWidget(chatTab);
				LOGGER_DEBUG("Changing focus on ChatTab for group {}.", group.toString());
			} else {
				LOGGER_DEBUG("Not changing focus on ChatTab for group {}, not open.", group.toString());
			}
		}

		void SimpleTabController::slotTabCloseRequested(int index) {
			QWidget* const widget = m_chatTabWidget->widget(index);
			ChatTab* const chatTab = dynamic_cast<ChatTab*>(widget);
			if (chatTab != nullptr) {
				QHash<openmittsu::protocol::ContactId, ChatTab*>::const_iterator itContacts = m_contactTabs.constBegin();
				QHash<openmittsu::protocol::ContactId, ChatTab*>::const_iterator endContacts = m_contactTabs.constEnd();
				for (; itContacts != endContacts; ++itContacts) {
					if (itContacts.value() == chatTab) {
						openmittsu::protocol::ContactId const contact = itContacts.key();
						LOGGER_DEBUG("ChatTab for contact {} is about to be closed, removing from index.", contact.toString());
						m_contactTabs.remove(contact);
						return;
					}
				}

				// Check groups next
				QHash<openmittsu::protocol::GroupId, ChatTab*>::const_iterator itGroups = m_groupTabs.constBegin();
				QHash<openmittsu::protocol::GroupId, ChatTab*>::const_iterator endGroups = m_groupTabs.constEnd();
				for (; itGroups != endGroups; ++itGroups) {
					if (itGroups.value() == chatTab) {
						openmittsu::protocol::GroupId const group = itGroups.key();
						LOGGER_DEBUG("ChatTab for group {} is about to be closed, removing from index.", group.toString());
						m_groupTabs.remove(group);
						return;
					}
				}

				LOGGER()->warn("Received closing warning for unknown ChatTab.");
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Could not cast widget pointer to ChatTab?!";
			}
		}

	}
}
