#ifndef OPENMITTSU_DATAPROVIDERS_BACKEDGROUPMESSAGE_H_
#define OPENMITTSU_DATAPROVIDERS_BACKEDGROUPMESSAGE_H_

#include <QString>
#include <QObject>
#include <memory>

#include "src/dataproviders/BackedMessage.h"
#include "src/dataproviders/messages/GroupMessage.h"
#include "src/dataproviders/MessageCenter.h"

namespace openmittsu {
	namespace dataproviders {

		class BackedGroupMessage : public BackedMessage {
			Q_OBJECT
		public:
			BackedGroupMessage(std::shared_ptr<messages::GroupMessage> const& message, BackedContact const& sender, openmittsu::dataproviders::MessageCenter& messageCenter);
			BackedGroupMessage(BackedGroupMessage const& other);
			virtual ~BackedGroupMessage();

			virtual void setIsSeen() override;

			messages::GroupMessageType getMessageType() const;
		protected:
			virtual messages::UserMessage const& getMessage() const override;
		private:
			std::shared_ptr<messages::GroupMessage> const m_message;
			openmittsu::dataproviders::MessageCenter& m_messageCenter;
		};

	}
}

#endif // OPENMITTSU_DATAPROVIDERS_BACKEDGROUPMESSAGE_H_
