#include "src/tasks/SafeCallbackTask.h"

#include "src/crypto/SafeMasterKey.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace tasks {

		SafeCallbackTask::SafeCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, QString const& userDefinedSafeServer, openmittsu::protocol::ContactId const& identity, QString const& safePassword, QString const& serviceUrl) : CertificateBasedCallbackTask(serverConfiguration->getSafeServerCertificateAsBase64()), CallbackTask(), m_safeMasterKey(openmittsu::crypto::SafeMasterKey::fromIdAndPassword(identity, safePassword)), m_urlString(makeUrl(userDefinedSafeServer, serverConfiguration->getSafeServerHost(), m_safeMasterKey, serviceUrl)), m_agentString(serverConfiguration->getSafeServerAgent()), m_identity(identity), m_safePassword(safePassword) {
			//
		}

		SafeCallbackTask::~SafeCallbackTask() {
			// Intentionally left empty.
		}

		QString SafeCallbackTask::makeUrl(QString const& userDefinedSafeServer, QString const& defaultSafeServer, openmittsu::crypto::SafeMasterKey const& safeMasterKey, QString const& serviceSubUrl) {
			QString safeServer = userDefinedSafeServer;
			if (safeServer.isEmpty() || safeServer.isNull()) {
				safeServer = defaultSafeServer;
			}

			// Replace a %1 by the first byte from the backup ID for load balancing
			if (safeServer.contains(QStringLiteral("%1"))) {
				QString const firstByte(safeMasterKey.getBackupId().left(1).toHex());
				safeServer = safeServer.arg(firstByte);
			}

			// Remove slashes from end
			while (safeServer.endsWith('/')) {
				safeServer = safeServer.left(safeServer.length() - 1);
			}

			QString subUrl = serviceSubUrl;
			// Replace %1 by the backup ID 
			if (subUrl.contains(QStringLiteral("%1"))) {
				subUrl = subUrl.arg(QString(safeMasterKey.getBackupId().toHex()));
			}

			return safeServer.append(subUrl);
		}

	}
}
