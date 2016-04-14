#ifndef OPENMITTSU_MESSAGES_CONTACT_CONTACTMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_CONTACT_CONTACTMESSAGECONTENT_H_

#include "messages/MessageContent.h"

#include <QByteArray>

class CallbackTask;
class ServerConfiguration;

class ContactMessageContent : public MessageContent {
public:
	virtual ~ContactMessageContent();

	virtual ContactMessageContent* clone() const = 0;
};

#endif // OPENMITTSU_MESSAGES_CONTACT_CONTACTMESSAGECONTENT_H_
