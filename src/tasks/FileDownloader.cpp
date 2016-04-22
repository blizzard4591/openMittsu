#include "tasks/FileDownloader.h"

FileDownloader::FileDownloader(QUrl const& fileUrl, QObject *parent) : QObject(parent), downloadedData(), successful(false) {
	QObject::connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileDownloaded(QNetworkReply*)));

	QNetworkRequest request(fileUrl);
	networkAccessManager.get(request);
}

FileDownloader::~FileDownloader() {
	//
}

void FileDownloader::fileDownloaded(QNetworkReply* pReply) {
	if (pReply->error() == QNetworkReply::NoError) {
		downloadedData = pReply->readAll();
		successful = true;
	} else {
		successful = false;
	}
	// emit "done" signal
	pReply->deleteLater();
	emit downloadFinished();
}

QByteArray const& FileDownloader::getDownloadedData() const {
	return downloadedData;
}

bool FileDownloader::wasDownloadSuccessful() const {
	return successful;
}
