#ifndef OPENMITTSU_UTILITY_THREADDELETER_H_
#define OPENMITTSU_UTILITY_THREADDELETER_H_

#include <type_traits>

#include <QThread>

#include "utility/QObjectConnectionMacro.h"

template<typename T>
class ThreadDeleter {
	static_assert(
		std::is_base_of<QThread, T>::value, "T must be a descendant of QThread");
public:
	void operator()(T* thread) {
		if (thread != nullptr) {
			QThread* qThread = dynamic_cast<QThread*>(thread);
			if (qThread->isFinished()) {
				delete thread;
			} else {
				OPENMITTSU_CONNECT_QUEUED(qThread, finished(), qThread, deleteLater());
			}
		}
	}
};

template<typename T>
using UniquePtrWithDelayedThreadDeletion = std::unique_ptr<T, ThreadDeleter<T>>;

#endif // OPENMITTSU_UTILITY_THREADDELETER_H_
