#ifndef OPENMITTSU_WIDGETS_CHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CHATWIDGETITEM_H_

#include <QWidget>
#include <QString>
#include <QByteArray>
#include <QDateTime>
#include <QMenu>
#include <QLabel>
#include <QVector>
#include <QPixmap>

#include <cstdint>

#include "src/protocol/ContactId.h"
#include "src/protocol/MessageId.h"

#include "src/dataproviders/BackedMessage.h"
#include "src/dataproviders/BackedContact.h"

namespace Ui {
	class ChatWidgetItem;
}

namespace openmittsu {
	namespace widgets {

		class ChatWidgetItem : public QWidget {
			Q_OBJECT
		public:
			ChatWidgetItem(std::shared_ptr<openmittsu::dataproviders::BackedContact> const& contact, bool isMessageFromUs, QWidget* parent = nullptr);
			virtual ~ChatWidgetItem();

			virtual QSize sizeHint() const override;
			virtual bool hasHeightForWidth() const override;
			virtual bool isMessageFromUs() const;
			virtual void setWasReadByUs();

			bool operator <(ChatWidgetItem const& other) const;
			bool operator <=(ChatWidgetItem const& other) const;
		public slots:
			virtual void showContextMenu(const QPoint& pos) = 0;
			virtual void onContactDataChanged();
			virtual void onMessageDataChanged();
			virtual void setOptimalSizeHint(int width);
		protected:
			void addWidget(QWidget* widget);

			virtual void setInnerAlignment();
			virtual void setFromLabel(QString const& from);
			virtual void setUserPic(QPixmap const& userPic);
			virtual void setStatusLine(QString const& status);

			virtual QString buildFromLabel();
			virtual QPixmap buildUserPic();
			virtual QString buildStatusLine();

			virtual openmittsu::dataproviders::BackedMessage& getMessage() = 0;
			virtual openmittsu::dataproviders::BackedMessage const& getMessage() const = 0;

			virtual void appendCustomContextMenuEntries(QPoint const& pos, QMenu& menu);
			virtual bool handleCustomContextMenuEntrySelection(QAction* selectedAction);
			virtual void copyToClipboard();

			static QString preprocessLinks(QString const& text);
			static void configureLabel(QLabel* label, int fontSize);

			std::shared_ptr<openmittsu::dataproviders::BackedContact> m_contact;
		private:
			QSize m_sizeHint;
			Ui::ChatWidgetItem* m_ui;
			bool const m_isMessageFromUs;
			QVector<QWidget*> m_registeredWidgets;

			bool m_hasUserPic;
			bool const m_hasFromLine;
			void updateUiVisibilityAndEdgeRounding();
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CHATWIDGETITEM_H_
