#ifndef OPENMITTSU_TASKS_FILEDOWNLOADERCALLBACKTASK_H_
#define OPENMITTSU_TASKS_FILEDOWNLOADERCALLBACKTASK_H_

#include "src/messages/Message.h"
#include "src/tasks/CallbackTask.h"

#include <QByteArray>
#include <QUrl>

namespace openmittsu {
	namespace tasks {

		class FileDownloaderCallbackTask : public CallbackTask {
		public:
			FileDownloaderCallbackTask(QUrl const& url);
			virtual ~FileDownloaderCallbackTask();

			QByteArray const& getDownloadedFile() const;
		protected:
			virtual void taskRun() override;
		private:
			QUrl const m_url;
			QByteArray m_result;
		};

	}
}

#endif // OPENMITTSU_TASKS_FILEDOWNLOADERCALLBACKTASK_H_
