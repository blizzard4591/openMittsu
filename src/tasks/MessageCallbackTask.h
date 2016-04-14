#ifndef OPENMITTSU_TASKS_MESSAGECALLBACKTASK_H_
#define OPENMITTSU_TASKS_MESSAGECALLBACKTASK_H_

#include "acknowledgments/AcknowledgmentProcessor.h"
#include "tasks/CallbackTask.h"

#include <memory>

class Message;

class MessageCallbackTask : public virtual CallbackTask {
public:
	virtual ~MessageCallbackTask();

	virtual Message const* getInitialMessage() const;
	virtual Message* getResultMessage() const;

	virtual std::shared_ptr<AcknowledgmentProcessor> const& getAcknowledgmentProcessor() const;
protected:
	MessageCallbackTask(Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor);
private:
	std::unique_ptr<Message> message;
	std::shared_ptr<AcknowledgmentProcessor> const acknowledgmentProcessor;
};

#endif // OPENMITTSU_TASKS_MESSAGECALLBACKTASK_H_