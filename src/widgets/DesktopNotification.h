#ifndef OPENMITTSU_WIDGETS_DESKTOPNOTIFICATION_H_
#define OPENMITTSU_WIDGETS_DESKTOPNOTIFICATION_H_

#include <QAction>
#include <QMainWindow>
#include <QMenu>
#include <QObject>
#include <QString>
#include <QSystemTrayIcon>

#include <memory>

#include "src/options/OptionReaderFactory.h"
#include "src/options/OptionReader.h"

namespace openmittsu {
	namespace widgets {

		class DesktopNotification : public QObject {
			Q_OBJECT
		public:
			DesktopNotification(QMainWindow* mainWindow, openmittsu::options::OptionReaderFactory const& optionReaderFactory);
			virtual ~DesktopNotification();

			bool isClosing() const;
		public slots:
			void showNotificationContact(QString const& contactName);
			void showNotificationGroup(QString const& groupName);
		private slots:
			void trayMenu_exitOnTriggered();
			void trayIcon_onActivated(QSystemTrayIcon::ActivationReason reason);
		private:
			QMainWindow* m_mainWindow;
			QSystemTrayIcon m_trayIcon;
			QMenu m_contextMenu;
			std::unique_ptr<openmittsu::options::OptionReader> m_optionReader;

			bool m_isClosing;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_DESKTOPNOTIFICATION_H_
