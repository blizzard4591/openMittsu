#ifndef OPENMITTSU_PROTOCOL_AUTHENTICATIONPACKET_H_
#define OPENMITTSU_PROTOCOL_AUTHENTICATIONPACKET_H_

#include "protocol/ContactId.h"
#include "protocol/CryptoBox.h"
#include "protocol/Nonce.h"

class AuthenticationPacket {
public:
	explicit AuthenticationPacket(ContactId const& sender, CryptoBox* cryptoBox);
	virtual ~AuthenticationPacket();

	virtual QByteArray toPacket() const;
private:
	ContactId const sender;
	CryptoBox* const cryptoBox;
};

#endif // OPENMITTSU_PROTOCOL_AUTHENTICATIONPACKET_H_
