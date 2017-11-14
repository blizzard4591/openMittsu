#include "src/tasks/BlobDownloaderCallbackTask.h"

#include "src/exceptions/CryptoException.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/QObjectConnectionMacro.h"
#include "src/utility/Logging.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QSslConfiguration>
#include <QSslCertificate>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

namespace openmittsu {
	namespace tasks {

		BlobDownloaderCallbackTask::BlobDownloaderCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, openmittsu::messages::Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& blobId) : CertificateBasedCallbackTask(serverConfiguration->getBlobServerCertificateAsBase64()), MessageCallbackTask(message, acknowledgmentProcessor), urlString(serverConfiguration->getBlobServerRequestDownloadUrl()), agentString(serverConfiguration->getBlobServerRequestAgent()), blobId(blobId) {
			if (urlString.isEmpty() || urlString.isNull()) {
				throw openmittsu::exceptions::IllegalArgumentException() << "No blob download URL available from server configuration.";
			}
		}

		BlobDownloaderCallbackTask::~BlobDownloaderCallbackTask() {
			// Intentionally left empty.
		}

		void BlobDownloaderCallbackTask::taskRun() {
			LOGGER_DEBUG("Running BlobDownloaderCallbackTask for BlobId {}.", QString(blobId.toHex()).toStdString());

			QNetworkAccessManager networkAccessManager;
			QEventLoop eventLoop;

			OPENMITTSU_CONNECT(&networkAccessManager, finished(QNetworkReply*), &eventLoop, quit());

			// the HTTPs request
			QNetworkRequest request;
			request.setSslConfiguration(getSslConfigurationWithCaCerts());
			request.setUrl(QUrl(urlString.arg(QString(blobId.left(1).toHex())).arg(QString(blobId.toHex()))));
			request.setRawHeader("User-Agent", agentString.toUtf8());

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

		QByteArray const& BlobDownloaderCallbackTask::getDownloadedBlob() const {
			if (!hasFinishedSuccessfully()) {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "This BlobDownloaderCallbackTask did not finish successfully. There is no resulting blob to be fetched.";
			}
			return result;
		}

	}
}
