#ifndef OPENMITTSU_WIDGETS_SIMPLEGROUPCHATTAB_H_
#define OPENMITTSU_WIDGETS_SIMPLEGROUPCHATTAB_H_

#include "src/widgets/SimpleChatTab.h"
#include "src/dataproviders/BackedGroup.h"

namespace Ui {
class SimpleGroupChatTab;
}

namespace openmittsu {
	namespace widgets {

		class SimpleGroupChatTab : public SimpleChatTab {
			Q_OBJECT
		public:
			explicit SimpleGroupChatTab(std::shared_ptr<openmittsu::dataproviders::BackedGroup> const& backedGroup, QWidget* parent = nullptr);
			virtual ~SimpleGroupChatTab();

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
			void onGroupDataChanged();
		private:
			std::shared_ptr<openmittsu::dataproviders::BackedGroup> m_group;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_SIMPLEGROUPCHATTAB_H_
