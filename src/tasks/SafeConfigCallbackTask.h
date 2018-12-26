#ifndef OPENMITTSU_TASKS_SAFECONFIGCALLBACKTASK_H_
#define OPENMITTSU_TASKS_SAFECONFIGCALLBACKTASK_H_

#include "src/network/ServerConfiguration.h"

#include "src/tasks/SafeCallbackTask.h"

#include <QString>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QSslCertificate>
#include <memory>

namespace openmittsu {
	namespace tasks {

		struct SafeConfig {
			std::size_t maxBackupBytes;
			std::int64_t retentionDays;
		};

		class SafeConfigCallbackTask : public SafeCallbackTask {
		public:
			SafeConfigCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, QString const& userDefinedSafeServer, openmittsu::protocol::ContactId const& identity, QString const& safePassword);
			virtual ~SafeConfigCallbackTask();

			SafeConfig const& getFetchedSafeConfig() const;
		protected:
			virtual void taskRun() override;
		private:
			SafeConfig m_fetchedConfig;
		};

	}
}

#endif // OPENMITTSU_TASKS_SAFECONFIGCALLBACKTASK_H_
