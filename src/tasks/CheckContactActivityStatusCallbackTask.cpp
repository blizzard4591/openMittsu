#include "src/tasks/CheckContactActivityStatusCallbackTask.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/protocol/ProtocolSpecs.h"
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

		CheckContactActivityStatusCallbackTask::CheckContactActivityStatusCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, QSet<openmittsu::protocol::ContactId> const& identitiesToCheck) : CertificateBasedCallbackTask(serverConfiguration->getApiServerCertificateAsBase64()), urlString(serverConfiguration->getApiServerCheckStatusForIdsUrl()), agentString(serverConfiguration->getApiServerAgent()), CallbackTask(), identitiesToFetch(identitiesToCheck) {
			if (urlString.isEmpty() || urlString.isNull()) {
				throw openmittsu::exceptions::IllegalArgumentException() << "No URL for checking ID status available from server configuration.";
			}
		}

		CheckContactActivityStatusCallbackTask::~CheckContactActivityStatusCallbackTask() {
			// Intentionally left empty.
		}

		void CheckContactActivityStatusCallbackTask::taskRun() {
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
			
			QSet<openmittsu::protocol::ContactId>::const_iterator i = identitiesToFetch.constBegin();
			QList<openmittsu::protocol::ContactId> identitiesToFetchWithConstantOrdering;
			while (i != identitiesToFetch.constEnd()) {
				openmittsu::protocol::ContactId const& id = *i;
				identitiesToFetchWithConstantOrdering.append(id);

				jsonIdentities.append(QJsonValue(id.toQString()));

				++i;
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
						if (identitiesToFetchWithConstantOrdering.size() != answerArray.size()) {
							finishedWithError(-5, "Member states has a different size then the requested array.");
							return;
						}

						for (int i = 0, size = answerArray.size(); i < size; ++i) {
							int code = answerArray.at(i).toInt(-1);

							if (code == 0) {
								fetchedFeatureLevels.insert(identitiesToFetchWithConstantOrdering.at(i), openmittsu::protocol::AccountStatus::STATUS_ACTIVE);
							} else if (code == 1) {
								fetchedFeatureLevels.insert(identitiesToFetchWithConstantOrdering.at(i), openmittsu::protocol::AccountStatus::STATUS_INACTIVE);
							} else if (code == 2) {
								fetchedFeatureLevels.insert(identitiesToFetchWithConstantOrdering.at(i), openmittsu::protocol::AccountStatus::STATUS_INVALID);
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

		QHash<openmittsu::protocol::ContactId, openmittsu::protocol::AccountStatus> const& CheckContactActivityStatusCallbackTask::getFetchedStatus() const {
			return fetchedFeatureLevels;
		}

		QSet<openmittsu::protocol::ContactId> const& CheckContactActivityStatusCallbackTask::getContactIds() const {
			return identitiesToFetch;
		}

	}
}
