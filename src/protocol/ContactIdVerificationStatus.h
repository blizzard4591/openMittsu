#ifndef OPENMITTSU_PROTOCOL_CONTACTIDVERIFICATIONSTATUS_H_
#define OPENMITTSU_PROTOCOL_CONTACTIDVERIFICATIONSTATUS_H_

#include <QString>

namespace openmittsu {
	namespace protocol {

		enum class ContactIdVerificationStatus {
			VERIFICATION_STATUS_UNVERIFIED,
			VERIFICATION_STATUS_SERVER_VERIFIED,
			VERIFICATION_STATUS_FULLY_VERIFIED,
		};

		class ContactIdVerificationStatusHelper {
		public:
			static ContactIdVerificationStatus fromQString(QString const& contactIdVerificationStatus);
			static QString toQString(ContactIdVerificationStatus const& contactIdVerificationStatus);
		};

	}
}

#endif // OPENMITTSU_PROTOCOL_CONTACTIDVERIFICATIONSTATUS_H_
