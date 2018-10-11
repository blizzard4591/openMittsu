#ifndef OPENMITTSU_WIDGETS_SIMPLECONTACTCHATTAB_H_
#define OPENMITTSU_WIDGETS_SIMPLECONTACTCHATTAB_H_

#include "src/dataproviders/BackedContact.h"
#include "src/widgets/SimpleChatTab.h"

#include "src/protocol/ContactId.h"

namespace Ui {
	class SimpleContactChatTab;
}

namespace openmittsu {
	namespace widgets {

		class SimpleContactChatTab : public SimpleChatTab {
			Q_OBJECT
		public:
			explicit SimpleContactChatTab(std::shared_ptr<openmittsu::dataproviders::BackedContact> const& contact, QWidget* parent = nullptr);
			virtual ~SimpleContactChatTab();

			virtual QString getTabName() override;
		protected:
			virtual void internalOnNewMessage(QString const& uuid) override;
			virtual bool sendText(QString const& text) override;
			virtual bool sendImage(QByteArray const& image, QString const& caption) override;
			virtual bool sendLocation(openmittsu::utility::Location const& location) override;
			virtual void sendUserTypingStatus(bool isTyping) override;

			virtual openmittsu::dataproviders::MessageSource& getMessageSource() override;

			virtual bool canUserAgree() const override;
		private slots:
			void onContactDataChanged();
			void onContactStartedTyping();
			void onContactStoppedTyping();
		private:
			std::shared_ptr<openmittsu::dataproviders::BackedContact> m_contact;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_SIMPLECONTACTCHATTAB_H_
