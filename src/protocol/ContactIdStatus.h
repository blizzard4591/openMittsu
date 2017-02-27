#ifndef OPENMITTSU_PROTOCOL_CONTACTIDSTATUS_H_
#define OPENMITTSU_PROTOCOL_CONTACTIDSTATUS_H_

enum class ContactIdStatus : int {
	STATUS_UNKNOWN = -1,
	STATUS_INVALID = 2, // Invalid means unregistered or deleted ID
	STATUS_INACTIVE = 1,
	STATUS_ACTIVE = 0,
};

class ContactIdStatusHelper {
public:
	static ContactIdStatus intToContactIdStatus(int contactIdStatus);
	static int contactIdStatusToInt(ContactIdStatus const& contactIdStatus);
};

#endif // OPENMITTSU_PROTOCOL_CONTACTIDSTATUS_H_
