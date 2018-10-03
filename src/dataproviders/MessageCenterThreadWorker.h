#ifndef OPENMITTSU_DATAPROVIDERS_MESSAGECENTERTHREADWORKER_H_
#define OPENMITTSU_DATAPROVIDERS_MESSAGECENTERTHREADWORKER_H_

#include <QDir>
#include <QString>
#include <QThread>

#include <memory>

#include "src/crypto/KeyPair.h"
#include "src/database/Database.h"
#include "src/database/DatabaseWrapperFactory.h"
#include "src/dataproviders/MessageCenter.h"
#include "src/protocol/ContactId.h"

namespace openmittsu {
	namespace widgets {
		class TabController;
	}

	namespace dataproviders {

		class MessageCenterThreadWorker : public QObject {
			Q_OBJECT
		public:
			virtual ~MessageCenterThreadWorker();
		public slots:
			bool createMessageCenter(openmittsu::database::DatabaseWrapperFactory const& databaseWrapperFactory, std::shared_ptr<openmittsu::widgets::TabController> const& tabController, std::shared_ptr<openmittsu::utility::OptionMaster> const& optionMaster);

			bool hasMessageCenter() const;
			std::shared_ptr<openmittsu::dataproviders::MessageCenter> getMessageCenter();
			std::shared_ptr<openmittsu::dataproviders::MessageCenter> const& getMessageCenter() const;
		private:
			std::shared_ptr<openmittsu::dataproviders::MessageCenter> m_messageCenter;
		};

	}
}

#endif // OPENMITTSU_DATAPROVIDERS_MESSAGECENTERTHREADWORKER_H_
