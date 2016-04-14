#ifndef OPENMITTSU_EXCEPTIONHANDLINGAPPLICATION_H_
#define OPENMITTSU_EXCEPTIONHANDLINGAPPLICATION_H_

#include <QApplication>
#include <QObject>
#include <QEvent>

class ExceptionHandlingApplication : public QApplication {
public:
	ExceptionHandlingApplication(int& argc, char** argv);
	virtual ~ExceptionHandlingApplication();

	virtual bool notify(QObject* receiver, QEvent* event) override;

	static void displayExceptionInfo(std::exception* e, bool displayQMessageBox = true);
};

#endif // OPENMITTSU_EXCEPTIONHANDLINGAPPLICATION_H_