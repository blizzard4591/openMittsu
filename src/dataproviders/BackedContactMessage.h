#ifndef OPENMITTSU_DATAPROVIDERS_BACKEDCONTACTMESSAGE_H_
#define OPENMITTSU_DATAPROVIDERS_BACKEDCONTACTMESSAGE_H_

#include <QString>
#include <QObject>
#include <memory>

#include "src/database/internal/DatabaseContactMessage.h"
#include "src/dataproviders/BackedMessage.h"

namespace openmittsu {
	namespace dataproviders {
		class BackedContactMessage : public BackedMessage {
			Q_OBJECT
		public:
			BackedContactMessage(std::shared_ptr<messages::ContactMessage> const& message, BackedContact const& sender, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter);
			BackedContactMessage(BackedContactMessage const& other);
			virtual ~BackedContactMessage();

			virtual void setIsSeen() override;
			virtual void setIsAgreed();
			virtual void setIsDisagreed();

			messages::ContactMessageType getMessageType() const;
		protected:
			virtual messages::UserMessage const& getMessage() const override;
		private:
			std::shared_ptr<messages::ContactMessage> const m_message;
			openmittsu::dataproviders::MessageCenterWrapper m_messageCenter;
		};

	}
}

#endif // OPENMITTSU_DATAPROVIDERS_BACKEDCONTACTMESSAGE_H_
