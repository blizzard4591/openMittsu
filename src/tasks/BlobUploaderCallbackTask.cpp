#include "tasks/BlobUploaderCallbackTask.h"

#include "exceptions/IllegalArgumentException.h"
#include "exceptions/IllegalFunctionCallException.h"
#include "utility/QObjectConnectionMacro.h"
#include "utility/Logging.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpMultiPart>

BlobUploaderCallbackTask::BlobUploaderCallbackTask(ServerConfiguration* serverConfiguration, Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& dataToUpload) : CertificateBasedCallbackTask(serverConfiguration->getImageServerCertificateAsBase64()), MessageCallbackTask(message, acknowledgmentProcessor), urlString(serverConfiguration->getImageServerRequestUploadUrl()), agentString(serverConfiguration->getImageServerRequestAgent()), dataToUpload(dataToUpload) {
	if (urlString.isEmpty() || urlString.isNull()) {
		throw IllegalArgumentException() << "No image upload URL available from server configuration.";
	}
}

BlobUploaderCallbackTask::~BlobUploaderCallbackTask() {
	// Intentionally left empty.
}

void BlobUploaderCallbackTask::taskRun() {
	LOGGER_DEBUG("Running BlobUploaderCallbackTask for {} Bytes.", dataToUpload.size());

	QNetworkAccessManager networkAccessManager;
	QEventLoop eventLoop;

	OPENMITTSU_CONNECT(&networkAccessManager, finished(QNetworkReply*), &eventLoop, quit());

	// the HTTPs request
	QNetworkRequest request;
	request.setSslConfiguration(getSslConfigurationWithCaCerts());
	request.setUrl(QUrl(urlString));
	request.setRawHeader("User-Agent", agentString.toUtf8());

	/*
	Content-Disposition: form-data;
	name="blob";
	filename="blob.bin"
	Content-Type: application/octet-stream
	*/

	QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

	QHttpPart textPart;
	textPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
	textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"blob\"; filename=\"blob.bin\""));
	textPart.setBody(dataToUpload);

	multiPart->append(textPart);

	QNetworkReply *reply = networkAccessManager.post(request, multiPart);
	multiPart->setParent(reply);

	eventLoop.exec(); // blocks until "finished()" has been called

	if (reply->error() == QNetworkReply::NoError) {
		// success
		QByteArray answer(QByteArray::fromHex(reply->readAll()));
		delete reply;

		result = answer;
		finishedWithNoError();
	} else {
		// failure
		QString const errorString(reply->errorString());
		delete reply;
		finishedWithError(-1, errorString);
	}
}

QByteArray const& BlobUploaderCallbackTask::getBlobId() const {
	if (!hasFinishedSuccessfully()) {
		throw IllegalFunctionCallException() << "This BlobUploaderCallbackTask did not finish successfully. There is no resulting blob id to be fetched.";
	}
	return result;
}
