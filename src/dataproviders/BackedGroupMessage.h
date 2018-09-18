#ifndef OPENMITTSU_DATAPROVIDERS_BACKEDGROUPMESSAGE_H_
#define OPENMITTSU_DATAPROVIDERS_BACKEDGROUPMESSAGE_H_

#include <QString>
#include <QObject>
#include <memory>

#include "src/dataproviders/BackedMessage.h"
#include "src/dataproviders/MessageCenter.h"

#include "src/database/DatabaseReadonlyGroupMessage.h"

namespace openmittsu {
	namespace dataproviders {

		class BackedGroupMessage : public BackedMessage {
			Q_OBJECT
		public:
			BackedGroupMessage(openmittsu::database::DatabaseReadonlyGroupMessage const& message, BackedContact const& sender, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter);
			BackedGroupMessage(BackedGroupMessage const& other);
			virtual ~BackedGroupMessage();

			virtual void setIsSeen() override;

			messages::GroupMessageType getMessageType() const;
		protected:
			virtual messages::ReadonlyUserMessage const& getMessage() const override;
			virtual void loadCache() override;
		private:
			openmittsu::database::DatabaseReadonlyGroupMessage m_message;
			openmittsu::dataproviders::MessageCenterWrapper m_messageCenter;
		};

	}
}

#endif // OPENMITTSU_DATAPROVIDERS_BACKEDGROUPMESSAGE_H_
