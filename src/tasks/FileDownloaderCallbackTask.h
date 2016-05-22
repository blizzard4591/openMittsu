#ifndef OPENMITTSU_TASKS_FILEDOWNLOADERCALLBACKTASK_H_
#define OPENMITTSU_TASKS_FILEDOWNLOADERCALLBACKTASK_H_

#include "messages/Message.h"
#include "tasks/CallbackTask.h"

#include <QUrl>

class FileDownloaderCallbackTask : public CallbackTask {
public:
	FileDownloaderCallbackTask(QUrl const& url);
	virtual ~FileDownloaderCallbackTask();

	QByteArray const& getDownloadedFile() const;
protected:
	virtual void taskRun() override;
private:
	QUrl const url;

	QByteArray result;
};

#endif // OPENMITTSU_TASKS_FILEDOWNLOADERCALLBACKTASK_H_