#ifndef OPENMITTSU_MESSAGES_MESSAGECONTENTREGISTRY_H_
#define OPENMITTSU_MESSAGES_MESSAGECONTENTREGISTRY_H_

#include <unordered_map>
#include <memory>
#include <mutex>

namespace openmittsu {
	namespace messages {
		class MessageContentFactory;

		class MessageContentRegistry {
		public:
			bool registerContent(char signatureByte, std::shared_ptr<MessageContentFactory> const& messageContentFactory);

			std::shared_ptr<MessageContentFactory> getMessageContentFactoryForSignatureByte(char signatureByte);

			static MessageContentRegistry& getInstance();
		private:
			std::mutex mutex;
			std::unordered_map<char, std::shared_ptr<MessageContentFactory>> mappings;

			MessageContentRegistry();
			MessageContentRegistry(MessageContentRegistry const& other);
			virtual ~MessageContentRegistry() {}
		};

	}
}

#endif // OPENMITTSU_MESSAGES_MESSAGECONTENTREGISTRY_H_
