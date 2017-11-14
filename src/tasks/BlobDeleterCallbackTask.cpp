#include "src/tasks/BlobDeleterCallbackTask.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/QObjectConnectionMacro.h"
#include "src/utility/Logging.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpMultiPart>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

namespace openmittsu {
	namespace tasks {

		BlobDeleterCallbackTask::BlobDeleterCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, openmittsu::messages::Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& blobId) : CertificateBasedCallbackTask(serverConfiguration->getBlobServerCertificateAsBase64()), MessageCallbackTask(message, acknowledgmentProcessor), urlFinishedString(serverConfiguration->getBlobServerRequestDownloadFinishedUrl()), agentString(serverConfiguration->getBlobServerRequestAgent()), blobId(blobId) {
			if (urlFinishedString.isEmpty() || urlFinishedString.isNull()) {
				throw openmittsu::exceptions::IllegalArgumentException() << "No blob deletion URL available from server configuration.";
			}
		}

		BlobDeleterCallbackTask::~BlobDeleterCallbackTask() {
			// Intentionally left empty.
		}

		void BlobDeleterCallbackTask::taskRun() {
			LOGGER_DEBUG("Running BlobDeleterCallbackTask for BlobId {}.", QString(blobId.toHex()).toStdString());

			QNetworkAccessManager networkAccessManager;
			QEventLoop eventLoop;

			OPENMITTSU_CONNECT(&networkAccessManager, finished(QNetworkReply*), &eventLoop, quit());

			// the HTTPs request
			QNetworkRequest request;
			request.setSslConfiguration(getSslConfigurationWithCaCerts());
			request.setUrl(QUrl(urlFinishedString.arg(QString(blobId.left(1).toHex())).arg(QString(blobId.toHex()))));
			request.setRawHeader("User-Agent", agentString.toUtf8());

			QNetworkReply *reply = networkAccessManager.get(request);
			eventLoop.exec(); // blocks until "finished()" has been called

			if (reply->error() == QNetworkReply::NoError) {
				// success
				QByteArray answer(reply->readAll());
				delete reply;

				finishedWithNoError();
			} else {
				// failure
				QString const errorString(reply->errorString());
				delete reply;
				finishedWithError(-1, errorString);
			}
		}

	}
}
