#include "src/messages/MessageContentRegistry.h"

#include "src/messages/MessageContentFactory.h"

namespace openmittsu {
	namespace messages {

		MessageContentRegistry::MessageContentRegistry() {
			// Intentionally left empty.
		}

		MessageContentRegistry::MessageContentRegistry(MessageContentRegistry const& other) {
			throw;
		}

		bool MessageContentRegistry::registerContent(char signatureByte, std::shared_ptr<MessageContentFactory> const& messageContentFactory) {
			mutex.lock();
			mappings.insert(std::make_pair(signatureByte, messageContentFactory));
			mutex.unlock();

			return true;
		}

		std::shared_ptr<MessageContentFactory> MessageContentRegistry::getMessageContentFactoryForSignatureByte(char signatureByte) {
			mutex.lock();
			std::shared_ptr<MessageContentFactory> result = nullptr;
			if (mappings.find(signatureByte) != mappings.cend()) {
				result = mappings.find(signatureByte)->second;
			}
			mutex.unlock();

			return result;
		}

		MessageContentRegistry& MessageContentRegistry::getInstance() {
			static MessageContentRegistry instance;

			return instance;
		}

	}
}
