#ifndef OPENMITTSU_PROTOCOL_FIXEDNONCES_H_
#define OPENMITTSU_PROTOCOL_FIXEDNONCES_H_

#include "protocol/Nonce.h"

class FixedNonces {
public:
	static Nonce getFixedGroupImageNonce();
};

#endif // OPENMITTSU_PROTOCOL_FIXEDNONCES_H_
