#ifndef OPENMITTSU_UTILITY_THREADCONTAINER_H_
#define OPENMITTSU_UTILITY_THREADCONTAINER_H_

#include <QThread>

#include <memory>

#include "src/database/DatabaseThreadWorker.h"
#include "src/dataproviders/MessageCenterThreadWorker.h"

namespace openmittsu {
	namespace utility {

		template <typename T>
		class ThreadContainer : public QObject {
		public:
			ThreadContainer();
			virtual ~ThreadContainer();
		
			QObject* getQObjectPtr() const;
			T& getWorker();
			T const& getWorker() const;
		private:
			QThread m_workerThread;
			std::unique_ptr<T> m_threadWorker;
		};

		typedef ThreadContainer<openmittsu::database::DatabaseThreadWorker> DatabaseThreadContainer;
		typedef ThreadContainer<openmittsu::dataproviders::MessageCenterThreadWorker> MessageCenterThreadContainer;
	}
}

#endif // OPENMITTSU_UTILITY_THREADCONTAINER_H_
