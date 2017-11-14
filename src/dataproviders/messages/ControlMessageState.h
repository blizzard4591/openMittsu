#ifndef OPENMITTSU_DATAPROVIDERS_CONTROLMESSAGESTATE_H_
#define OPENMITTSU_DATAPROVIDERS_CONTROLMESSAGESTATE_H_

#include <QString>

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			enum class ControlMessageState {
				SENDFAILED, SENDING, SENT
			};

			class ControlMessageStateHelper {
			public:
				static QString toString(ControlMessageState const& messageState);
				static ControlMessageState fromString(QString const& messageStateString);

				static bool canSwitchTo(ControlMessageState const& originalState, ControlMessageState const& newState);
			};

		}
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_CONTROLMESSAGESTATE_H_
