#ifndef OPENMITTSU_MESSAGES_CONTACT_RECEIPTMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_CONTACT_RECEIPTMESSAGECONTENT_H_

#include "src/messages/MessageContentFactory.h"
#include "src/messages/contact/ContactMessageContent.h"
#include "src/protocol/MessageId.h"

#include <QByteArray>

#include <vector>

namespace openmittsu {
	namespace messages {
		namespace contact {

			class ReceiptMessageContent : public ContactMessageContent {
			public:
				enum class ReceiptType {
					RECEIVED,
					SEEN,
					AGREE,
					DISAGREE
				};

				ReceiptMessageContent(std::vector<openmittsu::protocol::MessageId> const& relatedMessages, ReceiptType const& receiptType);
				virtual ~ReceiptMessageContent();

				virtual ReceiptType const& getReceiptType() const;
				virtual std::vector<openmittsu::protocol::MessageId> const& getReferredMessageIds() const;

				virtual ContactMessageContent* clone() const override;

				virtual QByteArray toPacketPayload() const override;

				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				friend class TypedMessageContentFactory<ReceiptMessageContent>;
			private:
				std::vector<openmittsu::protocol::MessageId> const messageIds;
				ReceiptType const receiptType;

				char receiptTypeToChar(ReceiptType const& receiptType) const;
				ReceiptType charToReceiptType(char c) const;

				static bool registrationResult;

				ReceiptMessageContent();
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_CONTACT_RECEIPTMESSAGECONTENT_H_
