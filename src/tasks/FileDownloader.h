#ifndef OPENMITTSU_TASKS_FILEDOWNLOADER_H_
#define OPENMITTSU_TASKS_FILEDOWNLOADER_H_

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class FileDownloader : public QObject {
	Q_OBJECT
public:
	explicit FileDownloader(QUrl const& fileUrl, QObject *parent = nullptr);
	virtual ~FileDownloader();
	QByteArray const& getDownloadedData() const;
	bool wasDownloadSuccessful() const;
signals:
	void downloadFinished();

private slots:
	void fileDownloaded(QNetworkReply* pReply);

private:
	QNetworkAccessManager networkAccessManager;

	QByteArray downloadedData;
	bool successful;
};

#endif // OPENMITTSU_TASKS_FILEDOWNLOADER_H_
