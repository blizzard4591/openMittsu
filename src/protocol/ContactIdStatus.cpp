#include "protocol/ContactIdStatus.h"

ContactIdStatus ContactIdStatusHelper::intToContactIdStatus(int contactIdStatus) {
	switch (contactIdStatus) {
		case 0:
		case 1:
		case 2:
		case 3:
			return static_cast<ContactIdStatus>(contactIdStatus);
		default:
			return ContactIdStatus::STATUS_INVALID;
	}
}

int ContactIdStatusHelper::contactIdStatusToInt(ContactIdStatus const& contactIdStatus) {
	switch (contactIdStatus) {
		case ContactIdStatus::STATUS_ACTIVE:
		case ContactIdStatus::STATUS_INACTIVE:
		case ContactIdStatus::STATUS_INVALID:
		case ContactIdStatus::STATUS_UNKNOWN:
			return static_cast<int>(contactIdStatus);
		default:
			return static_cast<int>(ContactIdStatus::STATUS_INVALID);
	}
}
