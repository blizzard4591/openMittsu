#ifndef OPENMITTSU_MESSAGES_MESSAGECONTENTREGISTRY_H_
#define OPENMITTSU_MESSAGES_MESSAGECONTENTREGISTRY_H_

#include "MessageContentFactory.h"

#include <unordered_map>
#include <mutex>

class MessageContentRegistry {
public:
	bool registerContent(char signatureByte, MessageContentFactory* messageContentFactory);

	MessageContentFactory* getMessageContentFactoryForSignatureByte(char signatureByte);

	static MessageContentRegistry& getInstance();
private:
	std::mutex mutex;
	std::unordered_map<char, MessageContentFactory*> mappings;

	MessageContentRegistry();
	MessageContentRegistry(MessageContentRegistry const& other);
};

#endif // OPENMITTSU_MESSAGES_MESSAGECONTENTREGISTRY_H_
