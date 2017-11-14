#include "src/widgets/chat/ContactLocationChatWidgetItem.h"

#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QClipboard>

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace widgets {

		ContactLocationChatWidgetItem::ContactLocationChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent) : ContactChatWidgetItem(message, parent), m_lblLocation(new QLabel()) {
			if (message.getMessageType() != openmittsu::dataproviders::messages::ContactMessageType::LOCATION && !message.isStatusMessage()) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not handle message with type " << openmittsu::dataproviders::messages::ContactMessageTypeHelper::toString(message.getMessageType()) << ".";
			}

			ChatWidgetItem::configureLabel(m_lblLocation, 13);
			this->addWidget(m_lblLocation);

			onContactDataChanged();
			onMessageDataChanged();
		}

		ContactLocationChatWidgetItem::~ContactLocationChatWidgetItem() {
			delete m_lblLocation;
		}

		void ContactLocationChatWidgetItem::onMessageDataChanged() {
			openmittsu::utility::Location const location = m_contactMessage.getContentAsLocation();
			m_lblLocation->setOpenExternalLinks(true);
			QString const locationUrl = QString(QStringLiteral("https://maps.google.com/?q=%1,%2")).arg(location.getLatitude()).arg(location.getLongitude());
			m_lblLocation->setText(QString(tr("Location: <a href=\"%1\">%1</a> - %2")).arg(locationUrl).arg(location.getDescription()));

			ContactChatWidgetItem::onMessageDataChanged();
		}

		void ContactLocationChatWidgetItem::copyToClipboard() {
			QClipboard *clipboard = QApplication::clipboard();
			openmittsu::utility::Location const location = m_contactMessage.getContentAsLocation();

			clipboard->setText(QString(tr("Location: https://maps.google.com/?q=%1,%2 - %3")).arg(location.getLatitude()).arg(location.getLongitude()).arg(location.getDescription()));
		}

	}
}
