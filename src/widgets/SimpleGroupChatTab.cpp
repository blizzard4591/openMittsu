#include "src/widgets/SimpleGroupChatTab.h"

#include "src/dataproviders/MessageCenter.h"
#include "src/dataproviders/BackedGroupMessage.h"
#include "src/protocol/GroupId.h"
#include "src/messages/PreliminaryMessageFactory.h"

#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"

#include "src/widgets/chat/GroupImageChatWidgetItem.h"
#include "src/widgets/chat/GroupLocationChatWidgetItem.h"
#include "src/widgets/chat/GroupStatusChatWidgetItem.h"
#include "src/widgets/chat/GroupTextChatWidgetItem.h"

namespace openmittsu {
	namespace widgets {

		SimpleGroupChatTab::SimpleGroupChatTab(std::shared_ptr<openmittsu::dataproviders::BackedGroup> const& backedGroup, QWidget* parent) : SimpleChatTab(parent), m_group(backedGroup) {
			OPENMITTSU_CONNECT(m_group.get(), groupDataChanged(), this, onGroupDataChanged());
			OPENMITTSU_CONNECT(m_group.get(), newMessageAvailable(QString const&), this, onNewMessage(QString const&));
		}

		SimpleGroupChatTab::~SimpleGroupChatTab() {
			//
		}

		bool SimpleGroupChatTab::sendText(QString const& text) {
			return m_group->sendTextMessage(text);
		}

		bool SimpleGroupChatTab::sendImage(QByteArray const& image, QString const& caption) {
			return m_group->sendImageMessage(image, caption);
		}

		bool SimpleGroupChatTab::sendLocation(openmittsu::utility::Location const& location) {
			return m_group->sendLocationMessage(location);
		}

		void SimpleGroupChatTab::sendUserTypingStatus(bool) {
			return;
		}

		openmittsu::dataproviders::MessageSource& SimpleGroupChatTab::getMessageSource() {
			return *m_group;
		}

		bool SimpleGroupChatTab::canUserAgree() const {
			return false;
		}

		void SimpleGroupChatTab::internalOnNewMessage(QString const& uuid) {
			{
				QMutexLocker mutexLock(&m_knownUuidsMutex);
				if (m_knownUuids.contains(uuid)) {
					return;
				}
				m_knownUuids.insert(uuid);
			}
			setMessageCount(m_group->getMessageCount());

			openmittsu::dataproviders::BackedGroupMessage message = m_group->getMessageByUuid(uuid);
			openmittsu::dataproviders::messages::GroupMessageType messageType = message.getMessageType();
			switch (messageType) {
				case openmittsu::dataproviders::messages::GroupMessageType::AUDIO:
					LOGGER()->warn("Can not create audio message item in GUI, not supported yet.");
					break;
				case openmittsu::dataproviders::messages::GroupMessageType::GROUP_CREATION:
					this->addChatWidgetItem(new GroupStatusChatWidgetItem(m_group->getMessageByUuid(uuid)));
					break;
				case openmittsu::dataproviders::messages::GroupMessageType::IMAGE:
					this->addChatWidgetItem(new GroupImageChatWidgetItem(m_group->getMessageByUuid(uuid)));
					break;
				case openmittsu::dataproviders::messages::GroupMessageType::LOCATION:
					this->addChatWidgetItem(new GroupLocationChatWidgetItem(m_group->getMessageByUuid(uuid)));
					break;
				case openmittsu::dataproviders::messages::GroupMessageType::POLL:
					LOGGER()->warn("Can not create poll message item in GUI, not supported yet.");
					break;
				case openmittsu::dataproviders::messages::GroupMessageType::SET_IMAGE:
					this->addChatWidgetItem(new GroupStatusChatWidgetItem(m_group->getMessageByUuid(uuid)));
					break;
				case openmittsu::dataproviders::messages::GroupMessageType::SET_TITLE:
					this->addChatWidgetItem(new GroupStatusChatWidgetItem(m_group->getMessageByUuid(uuid)));
					break;
				case openmittsu::dataproviders::messages::GroupMessageType::SYNC_REQUEST:
					this->addChatWidgetItem(new GroupStatusChatWidgetItem(m_group->getMessageByUuid(uuid)));
					break;
				case openmittsu::dataproviders::messages::GroupMessageType::TEXT:
					this->addChatWidgetItem(new GroupTextChatWidgetItem(m_group->getMessageByUuid(uuid)));
					break;
				case openmittsu::dataproviders::messages::GroupMessageType::VIDEO:
					LOGGER()->warn("Can not create video message item in GUI, not supported yet.");
					break;
				default:
					LOGGER()->error("Can not create message item in GUI, unhandled message type.");
			}
		}

		void SimpleGroupChatTab::onGroupDataChanged() {
			emit tabNameChanged(this);
		}

		QString SimpleGroupChatTab::getTabName() {
			return m_group->getTitle();
		}

	}
}
