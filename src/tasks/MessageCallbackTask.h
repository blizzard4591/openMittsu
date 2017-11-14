#ifndef OPENMITTSU_TASKS_MESSAGECALLBACKTASK_H_
#define OPENMITTSU_TASKS_MESSAGECALLBACKTASK_H_

#include "src/acknowledgments/AcknowledgmentProcessor.h"
#include "src/tasks/CallbackTask.h"

#include <memory>

namespace openmittsu {
	namespace messages {
		class Message;
	}
}

namespace openmittsu {
	namespace tasks {

		class MessageCallbackTask : public CallbackTask {
		public:
			virtual ~MessageCallbackTask();

			virtual openmittsu::messages::Message const* getInitialMessage() const;
			virtual openmittsu::messages::Message* getResultMessage() const;

			virtual std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& getAcknowledgmentProcessor() const;
		protected:
			MessageCallbackTask(openmittsu::messages::Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor);
		private:
			std::unique_ptr<openmittsu::messages::Message> message;
			std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const acknowledgmentProcessor;
		};

	}
}

#endif // OPENMITTSU_TASKS_MESSAGECALLBACKTASK_H_