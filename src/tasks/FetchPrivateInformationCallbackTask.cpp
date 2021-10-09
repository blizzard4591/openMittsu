#include "src/tasks/FetchPrivateInformationCallbackTask.h"

#include "src/exceptions/CryptoException.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"

#include <memory>

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpMultiPart>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

namespace openmittsu {
	namespace tasks {

		FetchPrivateInformationCallbackTask::FetchPrivateInformationCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, openmittsu::crypto::BasicCryptoBox const& cryptoBox, openmittsu::protocol::ContactId const& ownId)
			: CertificateBasedCallbackTask(serverConfiguration->getApiServerCertificateAsBase64()), CallbackTask(), m_urlString(serverConfiguration->getApiServerFetchPrivateInformationForIdUrl()), m_agentString(serverConfiguration->getApiServerAgent()), m_cryptoBox(cryptoBox), m_contactId(ownId) {
			if (m_urlString.isEmpty() || m_urlString.isNull()) {
				throw openmittsu::exceptions::IllegalArgumentException() << "No feature level setting URL available from server configuration.";
			}
		}

		FetchPrivateInformationCallbackTask::~FetchPrivateInformationCallbackTask() {
			// Intentionally left empty.
		}

		void FetchPrivateInformationCallbackTask::taskRun() {
			QNetworkAccessManager networkAccessManager;
			QEventLoop eventLoop;

			OPENMITTSU_CONNECT(&networkAccessManager, finished(QNetworkReply*), &eventLoop, quit());

			std::unique_ptr<QNetworkReply> reply;
			{
				// the HTTPs request
				QNetworkRequest request;
				request.setSslConfiguration(getSslConfigurationWithCaCerts());
				request.setUrl(QUrl(m_urlString));
				request.setRawHeader("User-Agent", m_agentString.toUtf8());
				request.setRawHeader("Content-Type", "application/json");

				QJsonObject jsonObject;

				jsonObject.insert("identity", m_contactId.toQString());
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
				LOGGER_DEBUG("FetchPrivateInformation first response was: {}", QString(answer).toStdString());

				QJsonDocument answerDocument = QJsonDocument::fromJson(answer);
				if (answerDocument.isObject()) {
					QJsonObject answerObject = answerDocument.object();
					QJsonValue answerValuePubKey = answerObject.value("tokenRespKeyPub");
					QJsonValue answerValueToken = answerObject.value("token");
					QString const publicKeyHexString = QByteArray::fromBase64(answerValuePubKey.toString().toLatin1()).toHex();
					openmittsu::crypto::PublicKey const pubKey = openmittsu::crypto::PublicKey::fromHexString(publicKeyHexString);
					QByteArray const token = QByteArray::fromBase64(answerValueToken.toString().toLatin1());

					std::pair<openmittsu::crypto::Nonce, QByteArray> cryptoResult;
					try {
						cryptoResult = m_cryptoBox.encrypt(token, pubKey);
					} catch (openmittsu::exceptions::CryptoExceptionImpl const& ce) {
						finishedWithError(-3, QString("Could not encrypt token for API request: ").append(ce.what()));
					}

					QString const encryptedTokenBase64 = QString::fromLatin1(cryptoResult.second.toBase64());
					QString const nonceBase64 = QString::fromLatin1(cryptoResult.first.getNonce().toBase64());

					std::unique_ptr<QNetworkReply> replyReponse;

					{
						QJsonObject jsonObjectResponse;

						jsonObjectResponse.insert("identity", m_contactId.toQString());
						jsonObjectResponse.insert("token", answerValueToken);
						jsonObjectResponse.insert("nonce", nonceBase64);
						jsonObjectResponse.insert("response", encryptedTokenBase64);
						QJsonDocument jsonDocumentResponse(jsonObjectResponse);

						QByteArray jsonDataReponse = jsonDocumentResponse.toJson();
						QByteArray postDataResponseSize = QByteArray::number(jsonDataReponse.size());

						// the HTTPs request
						QNetworkRequest requestReponse;
						requestReponse.setSslConfiguration(getSslConfigurationWithCaCerts());
						requestReponse.setUrl(QUrl(m_urlString));
						requestReponse.setRawHeader("User-Agent", m_agentString.toUtf8());
						requestReponse.setRawHeader("Content-Type", "application/json");
						requestReponse.setRawHeader("Content-Length", postDataResponseSize);

						LOGGER_DEBUG("FetchPrivateInformation challenge response is: {}", QString(jsonDataReponse).toStdString());
						replyReponse.reset(networkAccessManager.post(requestReponse, jsonDataReponse));
					}

					eventLoop.exec(); // blocks until "finished()" has been called

					if (replyReponse->error() == QNetworkReply::NoError) {
						// success
						QByteArray answerResponse(replyReponse->readAll());
						LOGGER_DEBUG("FetchPrivateInformation second response was: {}", QString(answerResponse).toStdString());
						QJsonDocument answerResponseDocument = QJsonDocument::fromJson(answerResponse);
						if (answerResponseDocument.isObject()) {
							QJsonObject answerResponseObject = answerResponseDocument.object();
							QJsonValue answerResponseValueServerGroup = answerResponseObject.value("serverGroup");
							QJsonValue answerResponseValueSuccess = answerResponseObject.value("success");
							QJsonValue answerResponseValueError = answerResponseObject.value("error");

							bool success = answerResponseValueSuccess.toBool(false);
							if (!success) {
								finishedWithError(-6, QString("The operation failed, the server replied with the following error message: ").append(answerResponseValueError.toString()));
							} else {
								m_serverGroup = answerResponseValueServerGroup.toString();
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

		QString const& FetchPrivateInformationCallbackTask::getServerGroup() const {
			return m_serverGroup;
		}

	}
}
