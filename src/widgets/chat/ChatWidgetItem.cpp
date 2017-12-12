#include "src/widgets/chat/ChatWidgetItem.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"
#include "src/utility/TextFormatter.h"
#include "src/utility/QObjectConnectionMacro.h"

#include <QDateTime>
#include <QFont>
#include <QRegularExpression>

#include "ui_chatwidgetitem.h"

namespace openmittsu {
	namespace widgets {

		ChatWidgetItem::ChatWidgetItem(openmittsu::dataproviders::BackedContact const& contact, bool isMessageFromUs, QWidget* parent) : QWidget(parent), m_contact(contact), m_sizeHint(60, 400), m_ui(new Ui::ChatWidgetItem), m_isMessageFromUs(isMessageFromUs), m_hasUserPic(false), m_hasFromLine(!isMessageFromUs) {
			m_ui->setupUi(this);

			OPENMITTSU_CONNECT(&m_contact, contactDataChanged(), this, onContactDataChanged());

			// Connect the context menu
			setContextMenuPolicy(Qt::CustomContextMenu);
			if (!connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not connect signal customContextMenuRequested in ChatWidgetItem.";
			}
			// 250px minimal width
			this->setMinimumWidth(250);
			this->setMinimumHeight(60);
			this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
			m_sizeHint = QSize(60, 250);

			QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
			sizePolicy.setHeightForWidth(true);
			this->setSizePolicy(sizePolicy);

			// Correctly align content
			Qt::Alignment alignment;
			if (m_isMessageFromUs) {
				alignment = Qt::AlignRight;
			} else {
				alignment = Qt::AlignLeft;
			}

			m_ui->lblFrom->setAlignment(alignment);
			m_ui->lblTimeAndStatus->setAlignment(alignment);

			// Set visibility and edge rounding depending on user picture and from label visibility
			updateUiVisibilityAndEdgeRounding();
		}

		ChatWidgetItem::~ChatWidgetItem() {
			//
		}

		void ChatWidgetItem::updateUiVisibilityAndEdgeRounding() {
			QString upperStyleSheet(QStringLiteral("border-top-right-radius:5px;"));
			QString lowerStyleSheet(QStringLiteral("border-bottom-right-radius:5px;"));

			if (m_hasUserPic) {
				m_ui->lblUserPic->setVisible(true);
				m_ui->lblUserPic->setStyleSheet(QStringLiteral("border-top-left-radius:5px; border-bottom-left-radius:5px;"));
			} else {
				m_ui->lblUserPic->setVisible(false);
				upperStyleSheet.append(QStringLiteral("border-top-left-radius:5px;"));
				lowerStyleSheet.append(QStringLiteral("border-bottom-left-radius:5px;"));
			}

			if (m_hasFromLine) {
				m_ui->lblFrom->setVisible(true);
				m_ui->lblFrom->setStyleSheet(upperStyleSheet);
			} else {
				m_ui->lblFrom->setVisible(false);
				if (m_registeredWidgets.size() > 0) {
					m_registeredWidgets.at(0)->setStyleSheet(upperStyleSheet);
				} else {
					lowerStyleSheet.append(upperStyleSheet);
				}
			}
			m_ui->lblTimeAndStatus->setStyleSheet(lowerStyleSheet);
		}

		QSize ChatWidgetItem::sizeHint() const {
			return m_sizeHint;
		}

		void ChatWidgetItem::setOptimalSizeHint(int width) {
			m_sizeHint = QSize(width, heightForWidth(width));
			this->setMinimumHeight(heightForWidth(width));
			this->updateGeometry();
		}

		bool ChatWidgetItem::hasHeightForWidth() const {
			return true;
		}

		void ChatWidgetItem::appendCustomContextMenuEntries(QPoint const& pos, QMenu& menu) {
			// Empty base definition, override in subclass if needed.
		}

		bool ChatWidgetItem::handleCustomContextMenuEntrySelection(QAction* selectedAction) {
			// Empty base definition, override in subclass if needed.
			return false;
		}

		void ChatWidgetItem::copyToClipboard() {
			// Empty base definition, override in subclass if needed.
		}

		QString ChatWidgetItem::preprocessLinks(QString const& text) {
			QString result = openmittsu::utility::TextFormatter::format(text.toHtmlEscaped());

			return result;
		}

		bool ChatWidgetItem::operator <(ChatWidgetItem const& other) const {
			return getMessage() < other.getMessage();
		}

		bool ChatWidgetItem::operator <=(ChatWidgetItem const& other) const {
			return getMessage() <= other.getMessage();
		}

		void ChatWidgetItem::setInnerAlignment() {
			LOGGER_DEBUG("Setting inner Alignment on ChatWidgetItem.");
			Qt::Alignment alignment;
			if (m_isMessageFromUs) {
				alignment = Qt::AlignRight;
			} else {
				alignment = Qt::AlignLeft;
			}

			m_ui->lblFrom->setAlignment(alignment);
			m_ui->lblTimeAndStatus->setAlignment(alignment);
			for (QWidget* w : m_registeredWidgets) {
				QLabel* l = dynamic_cast<QLabel*>(w);
				if (l != nullptr) {
					l->setAlignment(alignment);
				} else {
					LOGGER()->error("Could not set alignment on content widget in ChatWidgetItem, cast to QLabel failed?!");
				}
			}
		}

		void ChatWidgetItem::addWidget(QWidget* widget) {
			m_registeredWidgets.append(widget);
			
			Qt::Alignment alignment;
			if (m_isMessageFromUs) {
				alignment = Qt::AlignRight;
			} else {
				alignment = Qt::AlignLeft;
			}

			{
				QLabel* lblPtr = dynamic_cast<QLabel*>(widget);
				if (lblPtr != nullptr) {
					lblPtr->setAlignment(alignment);
				}
			}

			m_ui->layInnerVertical->insertWidget(m_registeredWidgets.size(), widget, 0, 0);

			updateUiVisibilityAndEdgeRounding();
		}

		void ChatWidgetItem::setFromLabel(QString const& from) {
			if (!m_isMessageFromUs) {
				m_ui->lblFrom->setText(from);
			}
		}

		void ChatWidgetItem::setUserPic(QPixmap const& userPic) {
			if (m_isMessageFromUs || userPic.isNull()) {
				m_ui->lblUserPic->setVisible(false);
				m_hasUserPic = false;
			} else {
				m_ui->lblUserPic->setVisible(true);
				m_ui->lblUserPic->setPixmap(userPic);
				m_hasUserPic = true;
			}

			updateUiVisibilityAndEdgeRounding();
		}

		void ChatWidgetItem::setStatusLine(QString const& status) {
			m_ui->lblTimeAndStatus->setText(status);
		}

		void ChatWidgetItem::onMessageDataChanged() {
			setStatusLine(buildStatusLine());
		}

		void ChatWidgetItem::onContactDataChanged() {
			if (!m_isMessageFromUs) {
				setFromLabel(buildFromLabel());
				setUserPic(buildUserPic());
			}
		}

		QString ChatWidgetItem::buildFromLabel() {
			if (m_isMessageFromUs) {
				throw openmittsu::exceptions::InternalErrorException() << "ChatWidgetItem::buildFromLabel() was called on a message originating from us!";
			} else {
				return m_contact.getName();
			}
		}

		QPixmap ChatWidgetItem::buildUserPic() {
			return QPixmap();
		}

		QString ChatWidgetItem::buildStatusLine() {
			openmittsu::protocol::MessageTime const time = getMessage().getSentAt();
			bool const isMessageFromUs = getMessage().isMessageFromUs();
			if (isMessageFromUs) {
				//time = getMessage().getSentAt();
			} else {
				//time = getMessage().getReceivedAt();
			}

			QString timeString;
			if (!time.isNull()) {
				QDateTime const dateTime = time.getTime();
				if (dateTime.date() == QDate::currentDate()) {
					timeString = time.getTime().toString(QStringLiteral("HH:mm:ss"));
				} else {
					timeString = time.getTime().toString(QStringLiteral("HH:mm:ss, dd.MM.yyyy"));
				}
			}

			QString statusString;
			if (isMessageFromUs) {
				if (!getMessage().isSent() || time.isNull()) {
					return QStringLiteral("Sending...");
				}
			}

			return statusString.append(timeString);
		}

		void ChatWidgetItem::configureLabel(QLabel* label, int fontSize) {
			QFont font;
			font.setFamily(QStringLiteral("Source Sans Pro"));
			font.setStyleStrategy(QFont::PreferAntialias);
			font.setPointSize(fontSize);
			label->setFont(font);
			label->setWordWrap(true);
			QSizePolicy sizePolicy = label->sizePolicy();
			sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
			label->setSizePolicy(sizePolicy);

			// Activate RichText parsing
			label->setTextFormat(Qt::RichText);
			// Make links accessible
			label->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard | Qt::TextSelectableByKeyboard | Qt::TextSelectableByMouse);
			label->setOpenExternalLinks(true);
		}

		bool ChatWidgetItem::isMessageFromUs() const {
			return m_isMessageFromUs;
		}

		void ChatWidgetItem::setWasReadByUs() {
			if (!m_isMessageFromUs && !getMessage().isRead()) {
				getMessage().setIsSeen();
			}
		}

	}
}
