#include "src/dataproviders/messages/UserMessageState.h"

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			QString UserMessageStateHelper::toString(UserMessageState const& messageState) {
				switch (messageState) {
					case UserMessageState::SENDFAILED:
						return QStringLiteral("SENDFAILED");
					case UserMessageState::SENDING:
						return QStringLiteral("SENDING");
					case UserMessageState::SENT:
						return QStringLiteral("SENT");
					case UserMessageState::DELIVERED:
						return QStringLiteral("DELIVERED");
					case UserMessageState::READ:
						return QStringLiteral("READ");
					case UserMessageState::USERACK:
						return QStringLiteral("USERACK");
					case UserMessageState::USERDEC:
						return QStringLiteral("USERDEC");
					default:
						throw openmittsu::exceptions::InternalErrorException() << "Unhandled UserMessageState, this should never happen: " << static_cast<int>(messageState);
				}
			}

			UserMessageState UserMessageStateHelper::fromString(QString const& messageStateString) {
				if (messageStateString == QStringLiteral("SENDFAILED")) {
					return UserMessageState::SENDFAILED;
				} else if (messageStateString == QStringLiteral("SENDING")) {
					return UserMessageState::SENDING;
				} else if (messageStateString == QStringLiteral("SENT")) {
					return UserMessageState::SENT;
				} else if (messageStateString == QStringLiteral("DELIVERED")) {
					return UserMessageState::DELIVERED;
				} else if (messageStateString == QStringLiteral("PENDING")) { // This is a dirty workaround until I figure out why only group ballots are "pending"
					return UserMessageState::DELIVERED;
				} else if (messageStateString == QStringLiteral("READ")) {
					return UserMessageState::READ;
				} else if (messageStateString == QStringLiteral("USERACK")) {
					return UserMessageState::USERACK;
				} else if (messageStateString == QStringLiteral("USERDEC")) {
					return UserMessageState::USERDEC;
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Unhandled UserMessageState string representation, this should never happen: " << messageStateString.toStdString();
				}
			}

			bool UserMessageStateHelper::canSwitchTo(UserMessageState const& originalState, UserMessageState const& newState) {
				if (originalState == newState) {
					return true;
				}

				switch (originalState) {
					case UserMessageState::SENDFAILED:
						return (newState == UserMessageState::SENDING) || (newState == UserMessageState::SENT);
						break;
					case UserMessageState::SENDING:
						return (newState == UserMessageState::SENT) || (newState == UserMessageState::SENDFAILED);
						break;
					case UserMessageState::SENT:
						return (newState == UserMessageState::DELIVERED) || (newState == UserMessageState::READ) || (newState == UserMessageState::USERACK) || (newState == UserMessageState::USERDEC);
						break;
					case UserMessageState::DELIVERED:
						return (newState == UserMessageState::READ) || (newState == UserMessageState::USERACK) || (newState == UserMessageState::USERDEC);
						break;
					case UserMessageState::READ:
						return (newState == UserMessageState::USERACK) || (newState == UserMessageState::USERDEC);
						break;
					case UserMessageState::USERACK:
						return (newState == UserMessageState::USERDEC);
						break;
					case UserMessageState::USERDEC:
						return (newState == UserMessageState::USERACK);
						break;
					default:
						return false;
				}
			}

			bool UserMessageStateHelper::isSent(UserMessageState const& state) {
				return ((state == UserMessageState::SENT) || (state == UserMessageState::DELIVERED) || (state == UserMessageState::READ) || (state == UserMessageState::USERACK) || (state == UserMessageState::USERDEC));
			}

		}
	}
}
