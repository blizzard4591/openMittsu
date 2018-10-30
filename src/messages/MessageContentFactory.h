#ifndef OPENMITTSU_MESSAGES_MESSAGECONTENTFACTORY_H_
#define OPENMITTSU_MESSAGES_MESSAGECONTENTFACTORY_H_

#include <QByteArray>

namespace openmittsu {
	namespace messages {
		class FullMessageHeader;
		class MessageContent;

		class MessageContentFactory {
		public:
			virtual ~MessageContentFactory() {}

			virtual MessageContent* createFromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const = 0;
		};

		template<typename T>
		class TypedMessageContentFactory : public MessageContentFactory {
		public:
			virtual ~TypedMessageContentFactory() {}

			virtual MessageContent* createFromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override {
				T t;
				return t.fromPacketPayload(messageHeader, payload);
			}
		};

	}
}

#endif // OPENMITTSU_MESSAGES_MESSAGECONTENTFACTORY_H_
