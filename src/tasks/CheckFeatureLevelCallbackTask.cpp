#include "src/tasks/CheckFeatureLevelCallbackTask.h"

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

		CheckFeatureLevelCallbackTask::CheckFeatureLevelCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, QSet<openmittsu::protocol::ContactId> const& identitiesToCheck) : CertificateBasedCallbackTask(serverConfiguration->getApiServerCertificateAsBase64()), urlString(serverConfiguration->getApiServerFetchFeatureLevelsForIdsUrl()), agentString(serverConfiguration->getApiServerAgent()), CallbackTask(), identitiesToFetch(identitiesToCheck) {
			if (urlString.isEmpty() || urlString.isNull()) {
				throw openmittsu::exceptions::IllegalArgumentException() << "No feature level checking URL available from server configuration.";
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
				LOGGER_DEBUG("CheckFeatureLevel Response was: {}", QString(answer).toStdString());

				QJsonDocument answerDocument = QJsonDocument::fromJson(answer);
				if (answerDocument.isObject()) {
					QJsonObject answerObject = answerDocument.object();
					QJsonValue answerValue = answerObject.value("featureLevels");
					if (answerValue.isArray()) {
						QJsonArray answerArray = answerValue.toArray();
						if (identitiesToFetchWithConstantOrdering.size() != answerArray.size()) {
							finishedWithError(-5, "Member featureLevels has a different size then the requested array.");
							return;
						}

						for (int i = 0, size = answerArray.size(); i < size; ++i) {
							int const code = answerArray.at(i).toInt(-666);
							if (code == -666) {
								finishedWithError(-4, QString("Member featureLevels has an entry at position %1 that could not be converted to an int!").arg(i));
								return;
							}

							fetchedFeatureLevels.insert(identitiesToFetchWithConstantOrdering.at(i), openmittsu::protocol::FeatureLevelHelper::fromInt(code));
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

		QHash<openmittsu::protocol::ContactId, openmittsu::protocol::FeatureLevel> const& CheckFeatureLevelCallbackTask::getFetchedFeatureLevels() const {
			return fetchedFeatureLevels;
		}

		QSet<openmittsu::protocol::ContactId> const& CheckFeatureLevelCallbackTask::getContactIds() const {
			return identitiesToFetch;
		}

	}
}
