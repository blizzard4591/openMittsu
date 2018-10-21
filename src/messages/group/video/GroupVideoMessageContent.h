#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPVIDEOMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPVIDEOMESSAGECONTENT_H_

#include "src/messages/group/GroupMessageContent.h"

#include <QByteArray>

namespace openmittsu {
	namespace messages {
		namespace group {

			class GroupVideoMessageContent : public GroupMessageContent {
			public:
				GroupVideoMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& video, QByteArray const& image, quint16 lengthInSeconds);
				virtual ~GroupVideoMessageContent();

				virtual GroupMessageContent* clone() const override;

				virtual bool hasPreSendCallbackTask() const override;

				virtual openmittsu::tasks::CallbackTask* getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const override;
				virtual MessageContent* integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const override;

				virtual QByteArray toPacketPayload() const override;
				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				QByteArray const& getVideoData() const;
				QByteArray const& getImageData() const;
				quint16 getLengthInSeconds() const;
			private:
				QByteArray const m_videoData;
				QByteArray const m_imageData;
				quint16 const m_lengthInSeconds;

				static bool m_registrationResult;
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPVIDEOMESSAGECONTENT_H_
