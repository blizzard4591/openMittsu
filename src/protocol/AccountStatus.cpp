#include "src/protocol/AccountStatus.h"

namespace openmittsu {
	namespace protocol {

		AccountStatus AccountStatusHelper::fromInt(int contactIdStatus) {
			switch (contactIdStatus) {
				case 0:
				case 1:
				case 2:
				case -1:
					return static_cast<AccountStatus>(contactIdStatus);
				default:
					return AccountStatus::STATUS_INVALID;
			}
		}

		int AccountStatusHelper::toInt(AccountStatus const& contactIdStatus) {
			switch (contactIdStatus) {
				case AccountStatus::STATUS_ACTIVE:
				case AccountStatus::STATUS_INACTIVE:
				case AccountStatus::STATUS_INVALID:
				case AccountStatus::STATUS_UNKNOWN:
					return static_cast<int>(contactIdStatus);
				default:
					return static_cast<int>(AccountStatus::STATUS_INVALID);
			}
		}

	}
}
