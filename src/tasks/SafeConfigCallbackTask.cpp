#include "src/tasks/SafeConfigCallbackTask.h"

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

		SafeConfigCallbackTask::SafeConfigCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, QString const& userDefinedSafeServer, openmittsu::protocol::ContactId const& identity, QString const& safePassword) : SafeCallbackTask(serverConfiguration, userDefinedSafeServer, identity, safePassword, serverConfiguration->getSafeServerConfigUrl()) {
			if (m_urlString.isEmpty() || m_urlString.isNull()) {
				throw openmittsu::exceptions::IllegalArgumentException() << "No safe config fetching URL available from server configuration.";
			}
		}

		SafeConfigCallbackTask::~SafeConfigCallbackTask() {
			// Intentionally left empty.
		}

		void SafeConfigCallbackTask::taskRun() {
			QNetworkAccessManager networkAccessManager;
			QEventLoop eventLoop;

			OPENMITTSU_CONNECT(&networkAccessManager, finished(QNetworkReply*), &eventLoop, quit());

			// Reset output
			m_fetchedConfig.maxBackupBytes = 0;
			m_fetchedConfig.retentionDays = 0;

			// the HTTPs request
			QNetworkRequest request;
			request.setSslConfiguration(getSslConfigurationWithCaCerts());
			request.setUrl(QUrl(m_urlString));
			request.setRawHeader("User-Agent", m_agentString.toUtf8());
			request.setRawHeader("Accept", "application/json");

			std::unique_ptr<QNetworkReply> reply(networkAccessManager.get(request));
			eventLoop.exec(); // blocks until "finished()" has been called

			if (reply->error() == QNetworkReply::NoError) {
				// success
				QByteArray answer(reply->readAll());
				LOGGER_DEBUG("FetchThreemaSafe config response was: {} - {}", reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), QString(answer).toStdString());

				finishedWithNoError();
			} else {
				// failure
				QString const errorString(reply->errorString());
				finishedWithError(-1, errorString);
			}
		}

		SafeConfig const& SafeConfigCallbackTask::getFetchedSafeConfig() const {
			return m_fetchedConfig;
		}

	}
}
