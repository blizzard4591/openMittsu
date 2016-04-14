#ifndef OPENMITTSU_MESSAGES_GROUP_PRELIMINARYGROUPMESSAGE_H_
#define OPENMITTSU_MESSAGES_GROUP_PRELIMINARYGROUPMESSAGE_H_

#include "messages/group/PreliminaryGroupMessageHeader.h"
#include "messages/group/GroupMessageContent.h"

#include <QMetaType>

#include <memory>

class GroupMessage;

class PreliminaryGroupMessage {
public:
	PreliminaryGroupMessage(PreliminaryGroupMessageHeader* preliminaryMessageHeader, GroupMessageContent* messageContent);
	PreliminaryGroupMessage(PreliminaryGroupMessage const& other);
	virtual ~PreliminaryGroupMessage();

	PreliminaryGroupMessageHeader const* getPreliminaryMessageHeader() const;
	GroupMessageContent const* getPreliminaryMessageContent() const;

	friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<PreliminaryGroupMessage, true>;
private:
	std::unique_ptr<PreliminaryGroupMessageHeader const> const preliminaryMessageHeader;
	std::unique_ptr<GroupMessageContent const> const messageContent;

	// Disable the default constructor
	PreliminaryGroupMessage();
};

Q_DECLARE_METATYPE(PreliminaryGroupMessage)

#endif // OPENMITTSU_MESSAGES_GROUP_PRELIMINARYGROUPMESSAGE_H_
