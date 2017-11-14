#ifndef OPENMITTSU_MESSAGES_GROUP_PRELIMINARYGROUPMESSAGE_H_
#define OPENMITTSU_MESSAGES_GROUP_PRELIMINARYGROUPMESSAGE_H_

#include "src/messages/group/PreliminaryGroupMessageHeader.h"
#include "src/messages/group/GroupMessageContent.h"
#include "src/protocol/ContactId.h"

#include <QMetaType>
#include <QSet>

#include <memory>

namespace openmittsu {
	namespace messages {
		namespace group {

			class PreliminaryGroupMessage {
			public:
				PreliminaryGroupMessage(PreliminaryGroupMessageHeader* preliminaryMessageHeader, GroupMessageContent* messageContent, QSet<openmittsu::protocol::ContactId> const& recipients);
				PreliminaryGroupMessage(PreliminaryGroupMessage const& other);
				virtual ~PreliminaryGroupMessage();

				PreliminaryGroupMessageHeader const* getPreliminaryMessageHeader() const;
				GroupMessageContent const* getPreliminaryMessageContent() const;
				QSet<openmittsu::protocol::ContactId> const& getRecipients() const;

				friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<PreliminaryGroupMessage, true>;
			private:
				std::unique_ptr<PreliminaryGroupMessageHeader const> const m_preliminaryMessageHeader;
				std::unique_ptr<GroupMessageContent const> const m_messageContent;
				QSet<openmittsu::protocol::ContactId> const m_recipients;

				// Disable the default constructor
				PreliminaryGroupMessage();
			};

		}
	}
}

Q_DECLARE_METATYPE(openmittsu::messages::group::PreliminaryGroupMessage)

#endif // OPENMITTSU_MESSAGES_GROUP_PRELIMINARYGROUPMESSAGE_H_
