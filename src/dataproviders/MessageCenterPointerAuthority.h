#ifndef OPENMITTSU_DATAPROVIDERS_MESSAGECENTERPOINTERAUTHORITY_H_
#define OPENMITTSU_DATAPROVIDERS_MESSAGECENTERPOINTERAUTHORITY_H_

#include <memory>

#include <QMutex>

#include "src/dataproviders/MessageCenter.h"
#include "src/dataproviders/MessageCenterWrapperFactory.h"

namespace openmittsu {
	namespace dataproviders {
		class MessageCenterWrapperFactory;

		class MessageCenterPointerAuthority : public QObject {
			Q_OBJECT
		public:
			MessageCenterPointerAuthority();
			virtual ~MessageCenterPointerAuthority();

		public slots:
			void setMessageCenter(std::shared_ptr<MessageCenter> newMessageCenter);

			std::weak_ptr<MessageCenter> getMessageCenterWeak() const;
			std::shared_ptr<MessageCenter> getMessageCenterStrong() const;

			MessageCenterWrapperFactory getMessageCenterWrapperFactory() const;
		signals:
			void newMessageCenterAvailable();
		private:
			std::shared_ptr<MessageCenter> m_messageCenter;
			mutable QMutex m_mutex;
		};

	}
}

#endif // OPENMITTSU_DATAPROVIDERS_MESSAGECENTERPOINTERAUTHORITY_H_
