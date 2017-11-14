#include "src/tasks/FileDownloaderCallbackTask.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/utility/QObjectConnectionMacro.h"
#include "src/utility/Logging.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace openmittsu {
	namespace tasks {

		FileDownloaderCallbackTask::FileDownloaderCallbackTask(QUrl const& url) : CallbackTask(), m_url(url) {
			// Intentionally left empty.
		}

		FileDownloaderCallbackTask::~FileDownloaderCallbackTask() {
			// Intentionally left empty.
		}

		void FileDownloaderCallbackTask::taskRun() {
			LOGGER_DEBUG("Running FileDownloaderCallbackTask for URL {}.", QString(m_url.toDisplayString()).toStdString());

			QNetworkAccessManager networkAccessManager;
			QEventLoop eventLoop;

			OPENMITTSU_CONNECT(&networkAccessManager, finished(QNetworkReply*), &eventLoop, quit());

			// the HTTPs request
			QNetworkRequest request;
			request.setUrl(m_url);

			QNetworkReply *reply = networkAccessManager.get(request);
			eventLoop.exec(); // blocks until "finished()" has been called

			if (reply->error() == QNetworkReply::NoError) {
				// success
				m_result = reply->readAll();
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
				throw openmittsu::exceptions::IllegalFunctionCallException() << "This FileDownloaderCallbackTask did not finish successfully. There is no resulting file to be fetched.";
			}
			return m_result;
		}

	}
}
