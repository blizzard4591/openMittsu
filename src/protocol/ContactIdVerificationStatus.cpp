#include "src/protocol/ContactIdVerificationStatus.h"

#include "src/exceptions/IllegalArgumentException.h"

namespace openmittsu {
	namespace protocol {

		ContactIdVerificationStatus ContactIdVerificationStatusHelper::fromQString(QString const& contactIdVerificationStatus) {
			if (contactIdVerificationStatus == QStringLiteral("UNVERIFIED")) {
				return ContactIdVerificationStatus::VERIFICATION_STATUS_UNVERIFIED;
			} else if (contactIdVerificationStatus == QStringLiteral("SERVER_VERIFIED")) {
				return ContactIdVerificationStatus::VERIFICATION_STATUS_SERVER_VERIFIED;
			} else if (contactIdVerificationStatus == QStringLiteral("FULLY_VERIFIED")) {
				return ContactIdVerificationStatus::VERIFICATION_STATUS_FULLY_VERIFIED;
			} else {
				throw openmittsu::exceptions::IllegalArgumentException() << "Unknown ContactIdVerificationStatus: " << contactIdVerificationStatus.toStdString();
			}
		}

		QString ContactIdVerificationStatusHelper::toQString(ContactIdVerificationStatus const& contactIdVerificationStatus) {
			switch (contactIdVerificationStatus) {
				case ContactIdVerificationStatus::VERIFICATION_STATUS_UNVERIFIED:
					return QStringLiteral("UNVERIFIED");
				case ContactIdVerificationStatus::VERIFICATION_STATUS_SERVER_VERIFIED:
					return QStringLiteral("SERVER_VERIFIED");
				case ContactIdVerificationStatus::VERIFICATION_STATUS_FULLY_VERIFIED:
					return QStringLiteral("FULLY_VERIFIED");
				default:
					throw openmittsu::exceptions::IllegalArgumentException() << "Unknown ContactIdVerificationStatus: " << static_cast<int>(contactIdVerificationStatus);
			}
		}

	}
}
