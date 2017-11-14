#include "src/dataproviders/messages/ControlMessageState.h"

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			QString ControlMessageStateHelper::toString(ControlMessageState const& messageState) {
				switch (messageState) {
					case ControlMessageState::SENDFAILED:
						return QStringLiteral("SENDFAILED");
					case ControlMessageState::SENDING:
						return QStringLiteral("SENDING");
					case ControlMessageState::SENT:
						return QStringLiteral("SENT");
					default:
						throw openmittsu::exceptions::InternalErrorException() << "Unhandled ControlMessageState, this should never happen: " << static_cast<int>(messageState);
				}
			}

			ControlMessageState ControlMessageStateHelper::fromString(QString const& messageStateString) {
				if (messageStateString == QStringLiteral("SENDFAILED")) {
					return ControlMessageState::SENDFAILED;
				} else if (messageStateString == QStringLiteral("SENDING")) {
					return ControlMessageState::SENDING;
				} else if (messageStateString == QStringLiteral("SENT")) {
					return ControlMessageState::SENT;
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Unhandled ControlMessageState string representation, this should never happen: " << messageStateString.toStdString();
				}
			}

			bool ControlMessageStateHelper::canSwitchTo(ControlMessageState const& originalState, ControlMessageState const& newState) {
				if (originalState == newState) {
					return true;
				}

				switch (originalState) {
					case ControlMessageState::SENDFAILED:
						return (newState == ControlMessageState::SENDING) || (newState == ControlMessageState::SENT);
						break;
					case ControlMessageState::SENDING:
						return (newState == ControlMessageState::SENT) || (newState == ControlMessageState::SENDFAILED);
						break;
					case ControlMessageState::SENT:
						return false;
						break;
					default:
						return false;
				}
			}

		}
	}
}
