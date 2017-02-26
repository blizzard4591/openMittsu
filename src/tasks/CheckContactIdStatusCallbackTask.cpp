#include "tasks/CheckContactIdStatusCallbackTask.h"

#include "exceptions/CryptoException.h"
#include "exceptions/IllegalArgumentException.h"
#include "exceptions/IllegalFunctionCallException.h"
#include "exceptions/ProtocolErrorException.h"
#include "protocol/CryptoBox.h"
#include "protocol/ProtocolSpecs.h"
#include "utility/Logging.h"
#include "utility/QObjectConnectionMacro.h"
#include "IdentityHelper.h"

#include <memory>

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpMultiPart>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

CheckContactIdStatusCallbackTask::CheckContactIdStatusCallbackTask(std::shared_ptr<ServerConfiguration> const& serverConfiguration, QList<ContactId> const& identitiesToCheck) : CertificateBasedCallbackTask(serverConfiguration->getApiServerCertificateAsBase64()), urlString(serverConfiguration->getApiServerCheckStatusForIdsUrl()), agentString(serverConfiguration->getApiServerAgent()), identitiesToFetch(identitiesToCheck) {
	if (urlString.isEmpty() || urlString.isNull()) {
		throw IllegalArgumentException() << "No URL for checking ID status available from server configuration.";
	}
}

CheckContactIdStatusCallbackTask::~CheckContactIdStatusCallbackTask() {
	// Intentionally left empty.
}

void CheckContactIdStatusCallbackTask::taskRun() {
	QNetworkAccessManager networkAccessManager;
	QEventLoop eventLoop;

	OPENMITTSU_CONNECT(&networkAccessManager, finished(QNetworkReply*), &eventLoop, quit());

	// the HTTPs request
	QNetworkRequest request;
	request.setSslConfiguration(getSslConfigurationWithCaCerts());
	request.setUrl(QUrl(urlString));
	request.setRawHeader("User-Agent", agentString.toUtf8());
	request.setRawHeader("Content-Type", "application/json");
	
	QJsonObject jsonObject;

	QJsonArray jsonIdentities;
	for (int i = 0, size = identitiesToFetch.size(); i < size; ++i) {
		ContactId const& id = identitiesToFetch.at(i);
		jsonIdentities.append(QJsonValue(id.toQString()));
	}
	jsonObject.insert("identities", jsonIdentities);
	QJsonDocument jsonDocument(jsonObject);	

	QByteArray jsonData = jsonDocument.toJson();
	QByteArray postDataSize = QByteArray::number(jsonData.size());
	request.setRawHeader("Content-Length", postDataSize);

	std::unique_ptr<QNetworkReply> reply(networkAccessManager.post(request, jsonData));

	// clear result
	fetchedFeatureLevels.clear();

	eventLoop.exec(); // blocks until "finished()" has been called

	if (reply->error() == QNetworkReply::NoError) {
		// success
		QByteArray answer(reply->readAll());
		LOGGER_DEBUG("CheckContactIds Response was: {}", QString(answer).toStdString());

		QJsonDocument answerDocument = QJsonDocument::fromJson(answer);
		if (answerDocument.isObject()) {
			QJsonObject answerObject = answerDocument.object();
			QJsonValue answerValue = answerObject.value("states");
			if (answerValue.isArray()) {
				QJsonArray answerArray = answerValue.toArray();
				if (identitiesToFetch.size() != answerArray.size()) {
					finishedWithError(-5, "Member states has a different size then the requested array.");
					return;
				}

				for (int i = 0, size = answerArray.size(); i < size; ++i) {
					int code = answerArray.at(i).toInt(-1);

					if (code == 0) {
						fetchedFeatureLevels.insert(identitiesToFetch.at(i), ContactIdStatus::STATUS_ACTIVE);
					} else if (code == 1) {
						fetchedFeatureLevels.insert(identitiesToFetch.at(i), ContactIdStatus::STATUS_INACTIVE);
					} else if (code == 2) {
						fetchedFeatureLevels.insert(identitiesToFetch.at(i), ContactIdStatus::STATUS_INVALID);
					} else {
						finishedWithError(-6, QString("Member states has an entry that could not be converted to a valid status: ").append(answerArray.at(i).toString()));
						return;
					}
				}
			} else {
				finishedWithError(-3, "Member states is not a JSON Array.");
			}
		} else {
			finishedWithError(-2, "Result is not a JSON Object.");
		}

		finishedWithNoError();
	} else {
		// failure
		QString const errorString(reply->errorString());
		finishedWithError(-1, errorString);
	}
}

QHash<ContactId, ContactIdStatus> const& CheckContactIdStatusCallbackTask::getFetchedStatus() const {
	return fetchedFeatureLevels;
}

QList<ContactId> const& CheckContactIdStatusCallbackTask::getContactIds() const {
	return identitiesToFetch;
}
