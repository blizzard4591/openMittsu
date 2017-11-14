#ifndef OPENMITTSU_TASKS_CALLBACKTASK_H_
#define OPENMITTSU_TASKS_CALLBACKTASK_H_

#include <QString>
#include <QThread>
#include <QObject>

namespace openmittsu {
	namespace tasks {

		class CallbackTask : public QThread {
			Q_OBJECT
		public:
			virtual ~CallbackTask();

			int getErrorCode() const;
			QString const& getErrorMessage() const;

			virtual void run() override;

			virtual bool hasFinished() const;
			virtual bool hasFinishedSuccessfully() const;
		signals:
			void finished(openmittsu::tasks::CallbackTask* callbackTask);
		protected:
			CallbackTask();

			virtual void preRunSetup();
			virtual void taskRun();

			void finishedWithNoError();
			void finishedWithError(int errorCode, QString const& errorMessage);
		private:
			int cb_errorCode;
			QString cb_errorMessage;
			bool cb_isFinished;
			bool cb_finishedSuccessfully;
		};

	}
}

#endif // OPENMITTSU_TASKS_CALLBACKTASK_H_