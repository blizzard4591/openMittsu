#ifndef OPENMITTSU_DATAPROVIDERS_USERMESSAGESTATE_H_
#define OPENMITTSU_DATAPROVIDERS_USERMESSAGESTATE_H_

#include <QString>

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			enum class UserMessageState {
				SENDFAILED, SENDING, SENT, DELIVERED, READ, USERACK, USERDEC
			};

			class UserMessageStateHelper {
			public:
				static QString toString(UserMessageState const& messageState);
				static UserMessageState fromString(QString const& messageStateString);

				static bool canSwitchTo(UserMessageState const& originalState, UserMessageState const& newState);
				static bool isSent(UserMessageState const& state);
			};

		}
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_USERMESSAGESTATE_H_
