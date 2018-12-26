#include "src/tasks/SafeFetchCallbackTask.h"

#include "src/backup/SafeBackup.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"

#include <sodium.h>

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

		SafeFetchCallbackTask::SafeFetchCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, QString const& userDefinedSafeServer, openmittsu::protocol::ContactId const& identity, QString const& safePassword) : SafeCallbackTask(serverConfiguration, userDefinedSafeServer, identity, safePassword, serverConfiguration->getSafeServerBackupUrl()) {
			if (m_urlString.isEmpty() || m_urlString.isNull()) {
				throw openmittsu::exceptions::IllegalArgumentException() << "No safe fetching URL available from server configuration.";
			}
		}

		SafeFetchCallbackTask::~SafeFetchCallbackTask() {
			// Intentionally left empty.
		}

		void SafeFetchCallbackTask::taskRun() {
			QNetworkAccessManager networkAccessManager;
			QEventLoop eventLoop;

			OPENMITTSU_CONNECT(&networkAccessManager, finished(QNetworkReply*), &eventLoop, quit());

			// the HTTPs request
			QNetworkRequest request;
			request.setSslConfiguration(getSslConfigurationWithCaCerts());
			request.setUrl(QUrl(m_urlString));
			request.setRawHeader("User-Agent", m_agentString.toUtf8());
			request.setRawHeader("Accept", "application/octet-stream");

			std::unique_ptr<QNetworkReply> reply(networkAccessManager.get(request));
			eventLoop.exec(); // blocks until "finished()" has been called
			LOGGER_DEBUG("HTTP Response code was {}.", reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
			if (reply->error() == QNetworkReply::NoError) {
				// success
				QByteArray answer(reply->readAll());
				LOGGER_DEBUG("FetchThreemaSafe Response was: {}", QString(answer).toStdString());
				openmittsu::backup::SafeBackup safeBackup(openmittsu::backup::SafeBackup::fromEncryptedBackup(answer, m_safeMasterKey.getEncryptionKey()));

				finishedWithNoError();
			} else {
				// failure
				QString const errorString(reply->errorString());
				finishedWithError(-1, errorString);
			}
		}

	}
}
