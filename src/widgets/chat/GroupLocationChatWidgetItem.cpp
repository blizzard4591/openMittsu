#include "GroupLocationChatWidgetItem.h"

#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QClipboard>

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace widgets {

		GroupLocationChatWidgetItem::GroupLocationChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent) : GroupChatWidgetItem(message, parent), m_lblText(new QLabel()) {
			if (message.getMessageType() != openmittsu::dataproviders::messages::GroupMessageType::LOCATION && !message.isStatusMessage()) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not handle message with type " << openmittsu::dataproviders::messages::GroupMessageTypeHelper::toString(message.getMessageType()) << ".";
			}

			ChatWidgetItem::configureLabel(m_lblText, 13);
			this->addWidget(m_lblText);

			onContactDataChanged();
			onMessageDataChanged();
		}

		GroupLocationChatWidgetItem::~GroupLocationChatWidgetItem() {
			delete m_lblText;
		}

		void GroupLocationChatWidgetItem::onMessageDataChanged() {
			openmittsu::utility::Location const location = m_groupMessage.getContentAsLocation();
			m_lblText->setOpenExternalLinks(true);
			QString const locationUrl = QString(QStringLiteral("https://maps.google.com/?q=%1,%2")).arg(location.getLatitude()).arg(location.getLongitude());
			m_lblText->setText(QString(tr("Location: <a href=\"%1\">%1</a> - %2")).arg(locationUrl).arg(location.getDescription()));

			GroupChatWidgetItem::onMessageDataChanged();
		}

		void GroupLocationChatWidgetItem::copyToClipboard() {
			QClipboard *clipboard = QApplication::clipboard();
			openmittsu::utility::Location const location = m_groupMessage.getContentAsLocation();

			clipboard->setText(QString(tr("Location: https://maps.google.com/?q=%1,%2 - %3")).arg(location.getLatitude()).arg(location.getLongitude()).arg(location.getDescription()));
		}

	}
}
