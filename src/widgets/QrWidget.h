#ifndef OPENMITTSU_WIDGETS_QRWIDGET_H_
#define OPENMITTSU_WIDGETS_QRWIDGET_H_

#include <QWidget>
#include <QString>

class QrWidget : public QWidget {
	Q_OBJECT
public:
	explicit QrWidget(QWidget* parent = nullptr);
	virtual ~QrWidget();

	void setQrDataString(QString const& newDataString);
protected:
	void paintEvent(QPaintEvent* paintEvent) override;
private:
	QString data;

	char* qrCodeImageData;
	int qrCodeImageWidth;
};

#endif // OPENMITTSU_WIDGETS_QRWIDGET_H_
