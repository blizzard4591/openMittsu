#include "tasks/FileDownloaderCallbackTask.h"

#include "exceptions/IllegalFunctionCallException.h"
#include "utility/QObjectConnectionMacro.h"
#include "utility/Logging.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>

FileDownloaderCallbackTask::FileDownloaderCallbackTask(QUrl const& url) : CallbackTask(), url(url) {
	// Intentionally left empty.
}

FileDownloaderCallbackTask::~FileDownloaderCallbackTask() {
	// Intentionally left empty.
}

void FileDownloaderCallbackTask::taskRun() {
	LOGGER_DEBUG("Running FileDownloaderCallbackTask for URL {}.", QString(url.toDisplayString()).toStdString());

	QNetworkAccessManager networkAccessManager;
	QEventLoop eventLoop;

	OPENMITTSU_CONNECT(&networkAccessManager, finished(QNetworkReply*), &eventLoop, quit());
	
	// the HTTPs request
	QNetworkRequest request;
	request.setUrl(url);

	QNetworkReply *reply = networkAccessManager.get(request);
	eventLoop.exec(); // blocks until "finished()" has been called

	if (reply->error() == QNetworkReply::NoError) {
		// success
		result = reply->readAll();
		delete reply;

		finishedWithNoError();
	} else {
		// failure
		QString const errorString(reply->errorString());
		delete reply;

		finishedWithError(-1, errorString);
	}
}

QByteArray const& FileDownloaderCallbackTask::getDownloadedFile() const {
	if (!hasFinishedSuccessfully()) {
		throw IllegalFunctionCallException() << "This FileDownloaderCallbackTask did not finish successfully. There is no resulting file to be fetched.";
	}
	return result;
}
