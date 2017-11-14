#include "src/tasks/MessageCallbackTask.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/Message.h"

namespace openmittsu {
	namespace tasks {

		MessageCallbackTask::MessageCallbackTask(openmittsu::messages::Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor) : CallbackTask(), message(message), acknowledgmentProcessor(acknowledgmentProcessor) {
			// Intentionally left empty.
		}

		MessageCallbackTask::~MessageCallbackTask() {
			// Intentionally left empty.
		}

		openmittsu::messages::Message const* MessageCallbackTask::getInitialMessage() const {
			return message.get();
		}

		openmittsu::messages::Message* MessageCallbackTask::getResultMessage() const {
			if (!hasFinishedSuccessfully()) {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "This CallbackTask has not finished successfully, can not build result message.";
			}
			return getInitialMessage()->withNewMessageContent(getInitialMessage()->getMessageContent()->integrateCallbackTaskResult(this));
		}

		std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& MessageCallbackTask::getAcknowledgmentProcessor() const {
			return acknowledgmentProcessor;
		}

	}
}
