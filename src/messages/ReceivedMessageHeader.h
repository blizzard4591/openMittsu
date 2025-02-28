#ifndef OPENMITTSU_MESSAGES_RECEIVEDMESSAGEHEADER_H_
#define OPENMITTSU_MESSAGES_RECEIVEDMESSAGEHEADER_H_

#include <QtGlobal>
#include <QByteArray>
#include <QString>
#include <QMetaType>
#include <QSharedPointer>

#include "src/messages/FullMessageHeader.h"

namespace openmittsu {
	namespace messages {
		class ReceivedMessageHeader {
		public:
			ReceivedMessageHeader();
			ReceivedMessageHeader(FullMessageHeader const& fullMessageHeader);
			ReceivedMessageHeader(FullMessageHeader const& fullMessageHeader, openmittsu::protocol::MessageTime const& timeReceived);
			virtual ~ReceivedMessageHeader();

			virtual openmittsu::protocol::ContactId const& getSender() const;
			virtual openmittsu::protocol::MessageId const& getMessageId() const;
			virtual openmittsu::protocol::MessageTime const& getTimeSent() const;
			virtual openmittsu::protocol::MessageTime const& getTimeReceived() const;
			virtual MessageFlags const& getFlags() const;
			virtual openmittsu::protocol::PushFromId const& getPushFromName() const;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
			friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<ReceivedMessageHeader, true>;
#endif
		private:
			openmittsu::protocol::ContactId sender;
			openmittsu::protocol::ContactId receiver;
			openmittsu::protocol::MessageId messageId;
			openmittsu::protocol::MessageTime timeSent;
			openmittsu::protocol::MessageTime timeReceived;
			MessageFlags flags;
			openmittsu::protocol::PushFromId pushFromName;
		};

	}
}

Q_DECLARE_METATYPE(openmittsu::messages::ReceivedMessageHeader)
Q_DECLARE_METATYPE(QSharedPointer<openmittsu::messages::ReceivedMessageHeader const>)

#endif // OPENMITTSU_MESSAGES_RECEIVEDMESSAGEHEADER_H_
