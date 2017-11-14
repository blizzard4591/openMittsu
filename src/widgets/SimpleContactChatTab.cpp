#include "src/widgets/SimpleContactChatTab.h"

#include "src/dataproviders/MessageCenter.h"
#include "src/dataproviders/BackedContactMessage.h"
#include "src/protocol/ContactId.h"

#include "src/utility/OptionMaster.h"
#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"

#include "src/widgets/chat/ContactImageChatWidgetItem.h"
#include "src/widgets/chat/ContactLocationChatWidgetItem.h"
#include "src/widgets/chat/ContactStatusChatWidgetItem.h"
#include "src/widgets/chat/ContactTextChatWidgetItem.h"

namespace openmittsu {
	namespace widgets {

		SimpleContactChatTab::SimpleContactChatTab(openmittsu::dataproviders::BackedContact const& contact, QWidget* parent) : SimpleChatTab(parent), m_contact(contact) {
			OPENMITTSU_CONNECT(&m_contact, contactDataChanged(), this, onContactDataChanged());
			OPENMITTSU_CONNECT(&m_contact, newMessageAvailable(QString const&), this, onNewMessage(QString const&));
			OPENMITTSU_CONNECT(&m_contact, contactStartedTyping(), this, onContactStartedTyping());
			OPENMITTSU_CONNECT(&m_contact, contactStoppedTyping(), this, onContactStoppedTyping());
		}

		SimpleContactChatTab::~SimpleContactChatTab() {
			//
		}

		bool SimpleContactChatTab::sendText(QString const& text) {
			return m_contact.sendTextMessage(text);
		}

		bool SimpleContactChatTab::sendImage(QByteArray const& image, QString const& caption) {
			return m_contact.sendImageMessage(image, caption);
		}

		bool SimpleContactChatTab::sendLocation(openmittsu::utility::Location const& location) {
			return m_contact.sendLocationMessage(location);
		}

		void SimpleContactChatTab::sendUserTypingStatus(bool isTyping) {
			m_contact.sendTypingNotification(!isTyping);
		}

		openmittsu::dataproviders::MessageSource& SimpleContactChatTab::getMessageSource() {
			return m_contact;
		}

		bool SimpleContactChatTab::canUserAgree() const {
			return true;
		}

		void SimpleContactChatTab::internalOnNewMessage(QString const& uuid) {
			{
				QMutexLocker mutexLock(&m_knownUuidsMutex);
				if (m_knownUuids.contains(uuid)) {
					return;
				}
				m_knownUuids.insert(uuid);
			}
			setMessageCount(m_contact.getMessageCount());

			openmittsu::dataproviders::BackedContactMessage message = m_contact.getMessageByUuid(uuid);
			openmittsu::dataproviders::messages::ContactMessageType messageType = message.getMessageType();
			switch (messageType) {
				case openmittsu::dataproviders::messages::ContactMessageType::AUDIO:
					LOGGER()->warn("Can not create audio message item in GUI, not supported yet.");
					break;
				case openmittsu::dataproviders::messages::ContactMessageType::IMAGE:
					this->addChatWidgetItem(new ContactImageChatWidgetItem(m_contact.getMessageByUuid(uuid)));
					break;
				case openmittsu::dataproviders::messages::ContactMessageType::LOCATION:
					this->addChatWidgetItem(new ContactLocationChatWidgetItem(m_contact.getMessageByUuid(uuid)));
					break;
				case openmittsu::dataproviders::messages::ContactMessageType::POLL:
					LOGGER()->warn("Can not create poll message item in GUI, not supported yet.");
					break;
				case openmittsu::dataproviders::messages::ContactMessageType::TEXT:
					this->addChatWidgetItem(new ContactTextChatWidgetItem(m_contact.getMessageByUuid(uuid)));
					break;
				case openmittsu::dataproviders::messages::ContactMessageType::VIDEO:
					LOGGER()->warn("Can not create video message item in GUI, not supported yet.");
					break;
				default:
					LOGGER()->error("Can not create message item in GUI, unhandled message type.");
			}
		}

		void SimpleContactChatTab::onContactDataChanged() {
			emit tabNameChanged(this);
		}

		void SimpleContactChatTab::onContactStartedTyping() {
			setStatusLine(tr("[%1] started typing").arg(QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss"))));
		}
		
		void SimpleContactChatTab::onContactStoppedTyping() {
			setStatusLine(tr("[%1] stopped typing").arg(QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss"))));
		}

		QString SimpleContactChatTab::getTabName() {
			return m_contact.getNickname();
		}

	}
}
