#ifndef OPENMITTSU_MESSAGES_MESSAGECONTENTREGISTRY_H_
#define OPENMITTSU_MESSAGES_MESSAGECONTENTREGISTRY_H_

#include <unordered_map>
#include <mutex>

namespace openmittsu {
	namespace messages {
		class MessageContentFactory;

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
			virtual ~MessageContentRegistry() {}
		};

	}
}

#endif // OPENMITTSU_MESSAGES_MESSAGECONTENTREGISTRY_H_
