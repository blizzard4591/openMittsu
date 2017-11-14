#include "src/tasks/IdentityReceiverCallbackTask.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpMultiPart>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

namespace openmittsu {
	namespace tasks {

		IdentityReceiverCallbackTask::IdentityReceiverCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, openmittsu::protocol::ContactId const& identityToFetch) : CertificateBasedCallbackTask(serverConfiguration->getApiServerCertificateAsBase64()), CallbackTask(), m_urlString(serverConfiguration->getApiServerFetchPublicKeyForIdUrl()), m_agentString(serverConfiguration->getApiServerAgent()), m_identityToFetch(identityToFetch) {
			if (m_urlString.isEmpty() || m_urlString.isNull()) {
				throw openmittsu::exceptions::IllegalArgumentException() << "No identity download URL available from server configuration.";
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
			request.setUrl(QUrl(m_urlString.arg(m_identityToFetch.toQString())));
			request.setRawHeader("User-Agent", m_agentString.toUtf8());

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

					m_fetchedPublicKey = openmittsu::crypto::PublicKey::fromHexString(publicKeyHexString);
					finishedWithNoError();
				} catch (...) {
					finishedWithError(-1, QString("Could not fetch public key for identity ").append(m_identityToFetch.toQString()).append("An error occurred while parsing the server reply: ").append(answer.toHex()));
				}
			} else {
				// failure
				QString const errorString(reply->errorString());
				delete reply;
				finishedWithError(-2, QString("Could not fetch public key for identity ").append(m_identityToFetch.toQString()).append("An error occurred while parsing the server reply: ").append(errorString));
			}
		}

		openmittsu::crypto::PublicKey const& IdentityReceiverCallbackTask::getFetchedPublicKey() const {
			return m_fetchedPublicKey;
		}

		openmittsu::protocol::ContactId const& IdentityReceiverCallbackTask::getContactIdOfFetchedPublicKey() const {
			return m_identityToFetch;
		}

	}
}
