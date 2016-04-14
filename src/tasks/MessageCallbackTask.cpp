#include "tasks/MessageCallbackTask.h"

#include "exceptions/IllegalFunctionCallException.h"
#include "messages/Message.h"

MessageCallbackTask::MessageCallbackTask(Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor) : CallbackTask(), message(message), acknowledgmentProcessor(acknowledgmentProcessor) {
	// Intentionally left empty.
}

MessageCallbackTask::~MessageCallbackTask() {
	// Intentionally left empty.
}

Message const* MessageCallbackTask::getInitialMessage() const {
	return message.get();
}

Message* MessageCallbackTask::getResultMessage() const {
	if (!hasFinishedSuccessfully()) {
		throw IllegalFunctionCallException() << "This CallbackTask has not finished successfully, can not build result message.";
	}
	return getInitialMessage()->withNewMessageContent(getInitialMessage()->getMessageContent()->integrateCallbackTaskResult(this));
}

std::shared_ptr<AcknowledgmentProcessor> const& MessageCallbackTask::getAcknowledgmentProcessor() const {
	return acknowledgmentProcessor;
}
