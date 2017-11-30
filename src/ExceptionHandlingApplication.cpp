#include "src/ExceptionHandlingApplication.h"

#include <iostream>
#include <QMessageBox>
#include <QThread>
#include "src/exceptions/BaseException.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace utility {

		ExceptionHandlingApplication::ExceptionHandlingApplication(int& argc, char** argv) : QApplication(argc, argv) {
			// Intentionally left empty.
		}

		ExceptionHandlingApplication::~ExceptionHandlingApplication() {
			// Intentionally left empty.
		}

		bool ExceptionHandlingApplication::notify(QObject* receiver, QEvent* event) {
			try {
				return QApplication::notify(receiver, event);
			} catch (std::exception& e) {
				displayExceptionInfo(&e);

				this->quit();
				return true;
			}
		}

		void ExceptionHandlingApplication::displayExceptionInfo(std::exception* e, bool displayQMessageBox) {
			if (QThread::currentThread() != QApplication::instance()->thread()) {
				LOGGER()->warn("Can not display MessageBox for Exception Handling: Not in main GUI thread.");
				displayQMessageBox = false;
			}

			openmittsu::exceptions::BaseException* baseException = dynamic_cast<openmittsu::exceptions::BaseException*>(e);
			if (baseException != nullptr) {
				LOGGER()->critical("Caught an exception of type {} in the main program loop with cause: {}", baseException->name(), baseException->what());
				if (displayQMessageBox) {
					QMessageBox::critical(nullptr, QString("Uncaught Exception"), QString("Caught an exception of type %1 in the main program loop with cause: %2\n\nThe program will now terminate.").arg(baseException->name()).arg(baseException->what()));
				} else {
					std::cerr << "Caught an exception of type " << baseException->name() << " in the main program loop with cause: " << baseException->what() << std::endl;
				}
			} else {
				LOGGER()->critical("Caught an exception in the main program loop with cause: {}", e->what());
				if (displayQMessageBox) {
					QMessageBox::critical(nullptr, QString("Uncaught Exception"), QString("Caught an exception in the main program loop with cause: %1\n\nThe program will now terminate.").arg(e->what()));
				} else {
					std::cerr << "Caught an exception in the main program loop with cause: " << e->what() << std::endl;
				}
			}
		}

	}
}
