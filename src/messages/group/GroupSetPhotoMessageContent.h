#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPSETPHOTOMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPSETPHOTOMESSAGECONTENT_H_

#include "src/messages/group/GroupMessageContent.h"

#include <QByteArray>

namespace openmittsu {
	namespace messages {
		namespace group {

			class GroupSetPhotoMessageContent : public GroupMessageContent {
			public:
				GroupSetPhotoMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& groupPhotoData);
				virtual ~GroupSetPhotoMessageContent();

				virtual GroupMessageContent* clone() const override;

				virtual bool hasPreSendCallbackTask() const override;

				virtual openmittsu::tasks::CallbackTask* getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const override;
				virtual MessageContent* integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const override;

				virtual QByteArray toPacketPayload() const override;
				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				QByteArray const& getGroupPhoto() const;
			private:
				QByteArray groupPhoto;

				static bool registrationResult;
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPIMAGEMESSAGECONTENT_H_
