#ifndef OPENMITTSU_MESSAGES_FULLMESSAGEHEADER_H_
#define OPENMITTSU_MESSAGES_FULLMESSAGEHEADER_H_

#include <QtGlobal>
#include <QByteArray>
#include <QString>
#include <QMetaType>
#include <QSharedPointer>

#include "src/messages/MessageFlags.h"

#include "src/protocol/ContactId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"
#include "src/protocol/PushFromId.h"

namespace openmittsu {
	namespace messages {
		namespace contact {
			class PreliminaryContactMessageHeader;
		}

		namespace group {
			class PreliminaryGroupMessageHeader;
		}

		class FullMessageHeader {
		public:
			FullMessageHeader(openmittsu::protocol::ContactId const& receiverId, openmittsu::protocol::MessageTime const& time, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, MessageFlags const& flags, openmittsu::protocol::PushFromId const& pushFromId);
			FullMessageHeader(contact::PreliminaryContactMessageHeader const* preliminaryHeader, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::PushFromId const& pushFromId);
			FullMessageHeader(group::PreliminaryGroupMessageHeader const* preliminaryHeader, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::PushFromId const& pushFromId);
			FullMessageHeader(FullMessageHeader const& messageHeader, openmittsu::protocol::ContactId const& replacementReceiver, openmittsu::protocol::MessageId const& replacementMessageId);
			FullMessageHeader(FullMessageHeader const& other);
			virtual ~FullMessageHeader();

			static FullMessageHeader fromHeaderData(QByteArray const& headerData);
			static int getFullMessageHeaderSize();

			virtual openmittsu::protocol::ContactId const& getSender() const;
			virtual openmittsu::protocol::ContactId const& getReceiver() const;
			virtual openmittsu::protocol::MessageId const& getMessageId() const;
			virtual openmittsu::protocol::MessageTime const& getTime() const;
			virtual MessageFlags const& getFlags() const;
			virtual openmittsu::protocol::PushFromId const& getPushFromName() const;

			virtual QByteArray toPacket() const;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
			friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<FullMessageHeader, true>;
#endif
		private:
			openmittsu::protocol::ContactId const sender;
			openmittsu::protocol::ContactId const receiver;
			openmittsu::protocol::MessageId const messageId;
			openmittsu::protocol::MessageTime const time;
			MessageFlags const flags;
			openmittsu::protocol::PushFromId const pushFromName;

			FullMessageHeader();
		};

	}
}

Q_DECLARE_METATYPE(openmittsu::messages::FullMessageHeader)
Q_DECLARE_METATYPE(QSharedPointer<openmittsu::messages::FullMessageHeader const>)

#endif // OPENMITTSU_MESSAGES_FULLMESSAGEHEADER_H_
