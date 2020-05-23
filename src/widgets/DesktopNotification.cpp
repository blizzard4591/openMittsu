#include "src/widgets/DesktopNotification.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"
#include "src/utility/Logging.h"

#include <QApplication>
#include <QIcon>

namespace openmittsu {
	namespace widgets {

		DesktopNotification::DesktopNotification(QMainWindow* mainWindow, openmittsu::options::OptionReaderFactory const& optionReaderFactory) : QObject(), m_mainWindow(mainWindow), m_trayIcon(QIcon(":/icons/icon.png"), this), m_contextMenu(), m_optionReader(optionReaderFactory.getOptionReader()), m_isClosing(false) {
			if (QSystemTrayIcon::isSystemTrayAvailable()) {
				LOGGER()->info("System Tray is available.");
				OPENMITTSU_CONNECT(&m_trayIcon, activated(QSystemTrayIcon::ActivationReason), this, trayIcon_onActivated(QSystemTrayIcon::ActivationReason));

				QAction* exitAction = new QAction(QString(tr("Close")), &m_contextMenu);
				OPENMITTSU_CONNECT(exitAction, triggered(bool), this, trayMenu_exitOnTriggered());

				m_contextMenu.addAction(exitAction);
				m_trayIcon.setContextMenu(&m_contextMenu);
				m_trayIcon.show();
			} else {
				LOGGER()->warn("System Tray is not available.");
			}

			if (QSystemTrayIcon::supportsMessages()) {
				LOGGER()->info("System Tray supports messages.");
			} else {
				LOGGER()->warn("System Tray does not support messages.");
			}
		}

		DesktopNotification::~DesktopNotification() {
			//
		}

		void DesktopNotification::showNotificationContact(QString const& contactName) {
			if (QSystemTrayIcon::supportsMessages()) {
				m_trayIcon.showMessage("OpenMittsu", QString(tr("New message from %1")).arg(contactName));
			}
		}

		void DesktopNotification::showNotificationGroup(QString const& groupName) {
			if (QSystemTrayIcon::supportsMessages()) {
				// Sadly, the handler currently does not know the contact sending the message...
				//m_trayIcon.showMessage("OpenMittsu", QString(tr("New message from %1 in group %2")).arg(contactName).arg(groupName));
				m_trayIcon.showMessage("OpenMittsu", QString(tr("New message in group %1")).arg(groupName));
			}
		}

		void DesktopNotification::trayMenu_exitOnTriggered() {
			m_isClosing = true;
			m_mainWindow->close();
			QApplication::exit(0);
		}

		bool DesktopNotification::isClosing() const {
			return m_isClosing;
		}

		void DesktopNotification::trayIcon_onActivated(QSystemTrayIcon::ActivationReason reason) {
			if (reason == QSystemTrayIcon::Trigger) {
				if (m_mainWindow->isVisible()) {
					LOGGER()->info("Minimizing to Tray.");
					m_mainWindow->hide();
				} else {
					LOGGER()->info("Maximizing from Tray.");
					m_mainWindow->show();
					m_mainWindow->activateWindow();
				}
			}
		}

	}
}
