#include "tasks/SetFeatureLevelCallbackTask.h"

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

SetFeatureLevelCallbackTask::SetFeatureLevelCallbackTask(std::shared_ptr<ServerConfiguration> const& serverConfiguration, std::shared_ptr<CryptoBox> const& cryptoBox, std::shared_ptr<ClientConfiguration> const& clientConfiguration, FeatureLevel const& featureLevel) : CertificateBasedCallbackTask(serverConfiguration->getApiServerCertificateAsBase64()), urlString(serverConfiguration->getApiServerSetFeatureLevelUrl()), agentString(serverConfiguration->getApiServerAgent()), cryptoBox(cryptoBox), contactId(clientConfiguration->getClientIdentity()), featureLevelToSet(featureLevel) {
	if (urlString.isEmpty() || urlString.isNull()) {
		throw IllegalArgumentException() << "No feature level setting URL available from server configuration.";
	}
}

SetFeatureLevelCallbackTask::~SetFeatureLevelCallbackTask() {
	// Intentionally left empty.
}

void SetFeatureLevelCallbackTask::taskRun() {
	QNetworkAccessManager networkAccessManager;
	QEventLoop eventLoop;

	OPENMITTSU_CONNECT(&networkAccessManager, finished(QNetworkReply*), &eventLoop, quit());

	std::unique_ptr<QNetworkReply> reply;
	{
		// the HTTPs request
		QNetworkRequest request;
		request.setSslConfiguration(getSslConfigurationWithCaCerts());
		request.setUrl(QUrl(urlString));
		request.setRawHeader("User-Agent", agentString.toUtf8());
		request.setRawHeader("Content-Type", "application/json");

		QJsonObject jsonObject;

		jsonObject.insert("identity", contactId.toQString());
		jsonObject.insert("featureLevel", FeatureLevelHelper::featureLevelToInt(featureLevelToSet));
		QJsonDocument jsonDocument(jsonObject);

		QByteArray jsonData = jsonDocument.toJson();
		QByteArray postDataSize = QByteArray::number(jsonData.size());
		request.setRawHeader("Content-Length", postDataSize);

		reply.reset(networkAccessManager.post(request, jsonData));
	}

	eventLoop.exec(); // blocks until "finished()" has been called

	if (reply->error() == QNetworkReply::NoError) {
		// success
		QByteArray answer(reply->readAll());
		LOGGER_DEBUG("SetFeatureLevel first response was: {}", QString(answer).toStdString());

		QJsonDocument answerDocument = QJsonDocument::fromJson(answer);
		if (answerDocument.isObject()) {
			QJsonObject answerObject = answerDocument.object();
			QJsonValue answerValuePubKey = answerObject.value("tokenRespKeyPub");
			QJsonValue answerValueToken = answerObject.value("token");
			QString const publicKeyHexString = QByteArray::fromBase64(answerValuePubKey.toString().toLatin1()).toHex();
			PublicKey const pubKey = PublicKey::fromHexString(publicKeyHexString);
			QByteArray const token = QByteArray::fromBase64(answerValueToken.toString().toLatin1());

			std::pair<Nonce, QByteArray> cryptoResult;
			try {
				cryptoResult = cryptoBox->encrypt(token, pubKey);
			} catch (CryptoException const& ce) {
				finishedWithError(-3, QString("Could not encrypt token for API request: ").append(ce.what()));
			}

			QString const encryptedTokenBase64 = QString::fromLatin1(cryptoResult.second.toBase64());
			QString const nonceBase64 = QString::fromLatin1(cryptoResult.first.getNonce().toBase64());

			std::unique_ptr<QNetworkReply> replyReponse;

			{
				QJsonObject jsonObjectResponse;

				jsonObjectResponse.insert("identity", contactId.toQString());
				jsonObjectResponse.insert("featureLevel", FeatureLevelHelper::featureLevelToInt(featureLevelToSet));

				jsonObjectResponse.insert("token", answerValueToken);
				jsonObjectResponse.insert("nonce", nonceBase64);
				jsonObjectResponse.insert("response", encryptedTokenBase64);
				QJsonDocument jsonDocumentResponse(jsonObjectResponse);

				QByteArray jsonDataReponse = jsonDocumentResponse.toJson();
				QByteArray postDataResponseSize = QByteArray::number(jsonDataReponse.size());

				// the HTTPs request
				QNetworkRequest requestReponse;
				requestReponse.setSslConfiguration(getSslConfigurationWithCaCerts());
				requestReponse.setUrl(QUrl(urlString));
				requestReponse.setRawHeader("User-Agent", agentString.toUtf8());
				requestReponse.setRawHeader("Content-Type", "application/json");
				requestReponse.setRawHeader("Content-Length", postDataResponseSize);

				LOGGER_DEBUG("SetFeatureLevel challenge response is: {}", QString(jsonDataReponse).toStdString());
				replyReponse.reset(networkAccessManager.post(requestReponse, jsonDataReponse));
			}

			eventLoop.exec(); // blocks until "finished()" has been called

			if (replyReponse->error() == QNetworkReply::NoError) {
				// success
				QByteArray answerResponse(replyReponse->readAll());
				LOGGER_DEBUG("SetFeatureLevel second response was: {}", QString(answerResponse).toStdString());
				QJsonDocument answerResponseDocument = QJsonDocument::fromJson(answerResponse);
				if (answerResponseDocument.isObject()) {
					QJsonObject answerResponseObject = answerResponseDocument.object();
					QJsonValue answerResponseValueSuccess = answerResponseObject.value("success");
					QJsonValue answerResponseValueError = answerResponseObject.value("error");

					bool success = answerResponseValueSuccess.toBool(false);
					if (!success) {
						finishedWithError(-6, QString("The operation failed, the server replied with the following error message: ").append(answerResponseValueError.toString()));
					}
				} else {
					finishedWithError(-5, "Result is not a JSON Object.");
				}
			} else {
				// failure
				QString const errorString(replyReponse->errorString());
				finishedWithError(-4, errorString);
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

FeatureLevel const& SetFeatureLevelCallbackTask::getFeatureLevelToSet() const {
	return featureLevelToSet;
}
