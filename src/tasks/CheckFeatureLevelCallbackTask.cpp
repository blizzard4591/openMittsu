#include "tasks/CheckFeatureLevelCallbackTask.h"

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

CheckFeatureLevelCallbackTask::CheckFeatureLevelCallbackTask(ServerConfiguration* serverConfiguration, QList<ContactId> const& identitiesToCheck) : CertificateBasedCallbackTask(serverConfiguration->getApiServerCertificateAsBase64()), urlString(serverConfiguration->getApiServerFetchFeatureLevelsForIdsUrl()), agentString(serverConfiguration->getApiServerAgent()), identitiesToFetch(identitiesToCheck) {
	if (urlString.isEmpty() || urlString.isNull()) {
		throw IllegalArgumentException() << "No identity download URL available from server configuration.";
	}
}

CheckFeatureLevelCallbackTask::~CheckFeatureLevelCallbackTask() {
	// Intentionally left empty.
}

void CheckFeatureLevelCallbackTask::taskRun() {
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
		LOGGER_DEBUG("CheckFeatureLevel Response was: {}", QString(answer).toStdString());

		QJsonDocument answerDocument = QJsonDocument::fromJson(answer);
		if (answerDocument.isObject()) {
			QJsonObject answerObject = answerDocument.object();
			QJsonValue answerValue = answerObject.value("featureLevels");
			if (answerValue.isArray()) {
				QJsonArray answerArray = answerValue.toArray();
				if (identitiesToFetch.size() != answerArray.size()) {
					finishedWithError(-5, "Member featureLevels has a different size then the requested array.");
					return;
				}

				for (int i = 0, size = answerArray.size(); i < size; ++i) {
					QString codeString = answerArray.at(i).toString();
					bool ok = false;
					int code = codeString.toInt(&ok);
					if (!ok) {
						finishedWithError(-4, "Member featureLevels has an entry that could not be converted to an int.");
						return;
					}

					fetchedFeatureLevels.insert(identitiesToFetch.at(i), code);
				}
			} else {
				finishedWithError(-3, "Member featureLevels is not a JSON Array.");
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

QHash<ContactId, int> const& CheckFeatureLevelCallbackTask::getFetchedFeatureLevels() const {
	return fetchedFeatureLevels;
}

QList<ContactId> const& CheckFeatureLevelCallbackTask::getContactIds() const {
	return identitiesToFetch;
}
