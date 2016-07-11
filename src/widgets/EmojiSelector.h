#ifndef EMOJISELECTOR_H
#define EMOJISELECTOR_H

#include <QWidget>

namespace Ui {
class EmojiSelector;
}

class EmojiSelector : public QWidget
{
    Q_OBJECT

public:
    explicit EmojiSelector(QWidget *parent = 0);
    ~EmojiSelector();

private:
    Ui::EmojiSelector *ui;
};

#endif // EMOJISELECTOR_H
