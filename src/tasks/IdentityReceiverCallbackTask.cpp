#include "tasks/IdentityReceiverCallbackTask.h"

#include "exceptions/CryptoException.h"
#include "exceptions/IllegalArgumentException.h"
#include "exceptions/IllegalFunctionCallException.h"
#include "exceptions/ProtocolErrorException.h"
#include "protocol/CryptoBox.h"
#include "protocol/ProtocolSpecs.h"
#include "utility/Logging.h"
#include "utility/QObjectConnectionMacro.h"
#include "IdentityHelper.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpMultiPart>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

IdentityReceiverCallbackTask::IdentityReceiverCallbackTask(ServerConfiguration* serverConfiguration, ContactId const& identityToFetch) : CertificateBasedCallbackTask(serverConfiguration->getApiServerCertificateAsBase64()), urlString(serverConfiguration->getApiServerFetchPublicKeyForIdUrl()), agentString(serverConfiguration->getApiServerAgent()), identityToFetch(identityToFetch) {
	if (urlString.isEmpty() || urlString.isNull()) {
		throw IllegalArgumentException() << "No identity download URL available from server configuration.";
	}
}

IdentityReceiverCallbackTask::~IdentityReceiverCallbackTask() {
	// Intentionally left empty.
}

void IdentityReceiverCallbackTask::taskRun() {
	QNetworkAccessManager networkAccessManager;
	QEventLoop eventLoop;

	OPENMITTSU_CONNECT(&networkAccessManager, finished(QNetworkReply*), &eventLoop, quit());

	// the HTTPs request
	QNetworkRequest request;
	request.setSslConfiguration(getSslConfigurationWithCaCerts());
	request.setUrl(QUrl(urlString.arg(identityToFetch.toQString())));
	request.setRawHeader("User-Agent", agentString.toUtf8());

	QNetworkReply *reply = networkAccessManager.get(request);
	eventLoop.exec(); // blocks until "finished()" has been called

	if (reply->error() == QNetworkReply::NoError) {
		// success
		QByteArray answer(reply->readAll());
		delete reply;

		try {
			QJsonDocument jsonResponse = QJsonDocument::fromJson(answer);
			QJsonObject jsonObject = jsonResponse.object();
			QJsonValue jsonValueIdentity = jsonObject.value("identity");
			QJsonValue jsonValuePublicKey = jsonObject.value("publicKey");
			QString const publicKeyHexString = QByteArray::fromBase64(jsonValuePublicKey.toString().toLatin1()).toHex();
			LOGGER_DEBUG("Received reply for identity = \"{}\" with public key = \"{}\".", jsonValueIdentity.toString().toStdString(), publicKeyHexString.toStdString());

			fetchedPublicKey = PublicKey::fromHexString(publicKeyHexString);
			finishedWithNoError();
		} catch (...) {
			finishedWithError(-1, QString("Could not fetch public key for identity ").append(identityToFetch.toQString()).append("An error occurred while parsing the server reply: ").append(answer.toHex()));
		}
	} else {
		// failure
		QString const errorString(reply->errorString());
		delete reply;
		finishedWithError(-2, QString("Could not fetch public key for identity ").append(identityToFetch.toQString()).append("An error occurred while parsing the server reply: ").append(errorString));
	}
}

PublicKey const& IdentityReceiverCallbackTask::getFetchedPublicKey() const {
	return fetchedPublicKey;
}

ContactId const& IdentityReceiverCallbackTask::getContactIdOfFetchedPublicKey() const {
	return identityToFetch;
}
