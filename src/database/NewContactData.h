#ifndef OPENMITTSU_DATABASE_NEWCONTACTDATA_H_
#define OPENMITTSU_DATABASE_NEWCONTACTDATA_H_

#include "src/crypto/PublicKey.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/AccountStatus.h"
#include "src/protocol/ContactIdVerificationStatus.h"
#include "src/protocol/FeatureLevel.h"

#include <QMetaType>
#include <QString>

namespace openmittsu {
	namespace database {
		struct NewContactData {
			NewContactData() = default;
			NewContactData(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& pubKey) : id(contact), publicKey(pubKey), verificationStatus(openmittsu::protocol::ContactIdVerificationStatus::VERIFICATION_STATUS_UNVERIFIED), firstName(""), lastName(""), nickName(""), color(0) {
				//
			}

			openmittsu::protocol::ContactId id;
			openmittsu::crypto::PublicKey publicKey;
			openmittsu::protocol::ContactIdVerificationStatus verificationStatus;
			QString firstName;
			QString lastName;
			QString nickName;
			int color;
		};
	}
}

Q_DECLARE_METATYPE(openmittsu::database::NewContactData)

#endif // OPENMITTSU_DATABASE_NEWCONTACTDATA_H_
