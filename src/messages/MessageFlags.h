#ifndef OPENMITTSU_MESSAGES_MESSAGEFLAGS_H_
#define OPENMITTSU_MESSAGES_MESSAGEFLAGS_H_

#include <QByteArray>

class MessageFlags {
public:
	MessageFlags();
	MessageFlags(char messageFlags);
	MessageFlags(bool pushMessage, bool noQueuing, bool noAckExpected, bool messageHasAlreadyBeenDelivered, bool groupMessage);
	MessageFlags(MessageFlags const& other);
	virtual ~MessageFlags();

	virtual char getFlags() const;

	virtual bool isPushMessage() const;
	virtual bool isNoQueueMessage() const;
	virtual bool isNoAckExpectedForMessage() const;
	virtual bool isMessageHasAlreadyBeenDelivered() const;
	virtual bool isGroupMessage() const;
private:
	bool const pushMessage;
	bool const noQueuing; 
	bool const noAckExpected;
	bool const messageHasAlreadyBeenDelivered;
	bool const groupMessage;

	static char boolsToByte(bool bitLsbPlus0, bool bitLsbPlus1, bool bitLsbPlus2, bool bitLsbPlus3, bool bitLsbPlus4, bool bitLsbPlus5, bool bitLsbPlus6, bool bitLsbPlus7);
	static bool byteToBool(char c, size_t position);
};

#endif // OPENMITTSU_MESSAGES_MESSAGEFLAGS_H_
