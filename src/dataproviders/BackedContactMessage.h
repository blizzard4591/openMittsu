#ifndef OPENMITTSU_DATAPROVIDERS_BACKEDCONTACTMESSAGE_H_
#define OPENMITTSU_DATAPROVIDERS_BACKEDCONTACTMESSAGE_H_

#include <QString>
#include <QObject>
#include <memory>

#include "src/database/DatabaseReadonlyContactMessage.h"
#include "src/dataproviders/BackedMessage.h"

namespace openmittsu {
	namespace dataproviders {
		class BackedContactMessage : public BackedMessage {
			Q_OBJECT
		public:
			BackedContactMessage(openmittsu::database::DatabaseReadonlyContactMessage const& message, BackedContact const& sender, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter);
			BackedContactMessage(BackedContactMessage const& other);
			virtual ~BackedContactMessage();

			virtual void setIsSeen() override;
			virtual void setIsAgreed();
			virtual void setIsDisagreed();

			messages::ContactMessageType getMessageType() const;
		protected:
			virtual messages::ReadonlyUserMessage const& getMessage() const override;
			virtual void loadCache() override;
		private:
			openmittsu::database::DatabaseReadonlyContactMessage m_message;
			openmittsu::dataproviders::MessageCenterWrapper m_messageCenter;
		};

	}
}

#endif // OPENMITTSU_DATAPROVIDERS_BACKEDCONTACTMESSAGE_H_
