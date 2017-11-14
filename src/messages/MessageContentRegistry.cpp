#include "src/messages/MessageContentRegistry.h"

#include "MessageContentFactory.h"

namespace openmittsu {
	namespace messages {

		MessageContentRegistry::MessageContentRegistry() {
			// Intentionally left empty.
		}

		MessageContentRegistry::MessageContentRegistry(MessageContentRegistry const& other) {
			throw;
		}

		bool MessageContentRegistry::registerContent(char signatureByte, MessageContentFactory* messageContentFactory) {
			mutex.lock();
			mappings.insert(std::make_pair(signatureByte, messageContentFactory));
			mutex.unlock();

			return true;
		}

		MessageContentFactory* MessageContentRegistry::getMessageContentFactoryForSignatureByte(char signatureByte) {
			mutex.lock();
			MessageContentFactory* result = nullptr;
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
