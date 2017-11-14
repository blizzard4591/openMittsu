#ifndef OPENMITTSU_PROTOCOL_GROUPSTATUS_H_
#define OPENMITTSU_PROTOCOL_GROUPSTATUS_H_

namespace openmittsu {
	namespace protocol {

		enum class GroupStatus {
			KNOWN,
			DELETED,
			TEMPORARY,
			UNKNOWN
		};

	}
}

#endif // OPENMITTSU_PROTOCOL_GROUPSTATUS_H_
