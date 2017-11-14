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

		CheckContactActivityStatusCallbackTask::CheckContactActivityStatusCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, QSet<openmittsu::protocol::ContactId> const& identitiesToCheck) : CertificateBasedCallbackTask(serverConfiguration->getApiServerCertificateAsBase64()), CallbackTask(), m_urlString(serverConfiguration->getApiServerCheckStatusForIdsUrl()), m_agentString(serverConfiguration->getApiServerAgent()), m_identitiesToFetch(identitiesToCheck) {
			if (m_urlString.isEmpty() || m_urlString.isNull()) {
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
			request.setUrl(QUrl(m_urlString));
			request.setRawHeader("User-Agent", m_agentString.toUtf8());
			request.setRawHeader("Content-Type", "application/json");

			QJsonObject jsonObject;

			QJsonArray jsonIdentities;
			
			auto it = m_identitiesToFetch.constBegin();
			auto const end = m_identitiesToFetch.constEnd();
			QList<openmittsu::protocol::ContactId> identitiesToFetchWithConstantOrdering;
			for (; it != end; ++it) {
				openmittsu::protocol::ContactId const& id = *it;
				identitiesToFetchWithConstantOrdering.append(id);

				jsonIdentities.append(QJsonValue(id.toQString()));
			}
			jsonObject.insert("identities", jsonIdentities);
			QJsonDocument jsonDocument(jsonObject);

			QByteArray jsonData = jsonDocument.toJson();
			QByteArray postDataSize = QByteArray::number(jsonData.size());
			request.setRawHeader("Content-Length", postDataSize);

			std::unique_ptr<QNetworkReply> reply(networkAccessManager.post(request, jsonData));

			// clear result
			m_fetchedFeatureLevels.clear();

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
								m_fetchedFeatureLevels.insert(identitiesToFetchWithConstantOrdering.at(i), openmittsu::protocol::AccountStatus::STATUS_ACTIVE);
							} else if (code == 1) {
								m_fetchedFeatureLevels.insert(identitiesToFetchWithConstantOrdering.at(i), openmittsu::protocol::AccountStatus::STATUS_INACTIVE);
							} else if (code == 2) {
								m_fetchedFeatureLevels.insert(identitiesToFetchWithConstantOrdering.at(i), openmittsu::protocol::AccountStatus::STATUS_INVALID);
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
			return m_fetchedFeatureLevels;
		}

		QSet<openmittsu::protocol::ContactId> const& CheckContactActivityStatusCallbackTask::getContactIds() const {
			return m_identitiesToFetch;
		}

	}
}
