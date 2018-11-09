#ifndef OPENMITTSU_WIDGETS_EMOJISELECTOR_H_
#define OPENMITTSU_WIDGETS_EMOJISELECTOR_H_

#include <QWidget>

#include <memory>

namespace Ui {
	class EmojiSelector;
}

namespace openmittsu {
	namespace widgets {

		class EmojiSelector : public QWidget {
			Q_OBJECT
		public:
			explicit EmojiSelector(QWidget* parent = nullptr);
			virtual ~EmojiSelector();
		signals:
			void emojiDoubleClicked(QString const& emoji);
		private slots:
			void labelTextDoubleClicked(QString const& selectedText);
		private:
			std::unique_ptr<Ui::EmojiSelector> m_ui;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_EMOJISELECTOR_H_
