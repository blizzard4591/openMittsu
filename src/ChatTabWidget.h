#ifndef OPENMITTSU_CHATTABWIDGET_H_
#define OPENMITTSU_CHATTABWIDGET_H_

#include <QTabWidget>
#include <QString>
#include <QByteArray>
#include <QHash>
#include <QTimer>
#include <cstdint>

class ChatTabWidget : public QTabWidget {
	Q_OBJECT
public:
	virtual ~ChatTabWidget() {}

	ChatTabWidget(QWidget* parent = nullptr);
	void setTabBlinking(int index, bool doBlink = true);
private:
	class IconSet {
	public:
		QIcon standardIcon;
		QIcon blinkIcon;
	};
	QHash<int, IconSet> indexToIconHashMap;
	QTimer blinkTimer;
	QColor standardColor;
	QColor blinkColor;
	bool isCurrentlyInBlink;
private slots:
	void blinkTimerOnTimer();
};

#endif // OPENMITTSU_CHATTABWIDGET_H_