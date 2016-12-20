#ifndef OPENMITTSU_WIDGETS_EMOJISELECTOR_H_
#define OPENMITTSU_WIDGETS_EMOJISELECTOR_H_
#include <QWidget>

namespace Ui {
class EmojiSelector;
}

class EmojiSelector : public QWidget {
    Q_OBJECT
public:
    explicit EmojiSelector(QWidget *parent = nullptr);
    ~EmojiSelector();
signals:
	void emojiDoubleClicked(QString const& emoji);
private slots:
	void labelTextDoubleClicked(QString const& selectedText);
private:
    Ui::EmojiSelector *ui;
};

#endif // OPENMITTSU_WIDGETS_EMOJISELECTOR_H_
