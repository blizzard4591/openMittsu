#include "src/messages/contact/ContactLocationMessageContent.h"

#include "src/exceptions/ProtocolErrorException.h"
#include "src/messages/MessageContentRegistry.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/ProtocolSpecs.h"

#include <QStringList>

namespace openmittsu {
	namespace messages {
		namespace contact {

			// Register this MessageContent with the MessageContentRegistry
			bool ContactLocationMessageContent::registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_CONTACT_LOCATION, new TypedMessageContentFactory<ContactLocationMessageContent>());


			ContactLocationMessageContent::ContactLocationMessageContent() : ContactMessageContent(), latitude(0.0), longitude(0.0), height(0.0), description() {
				// Only accessible and used by the MessageContentFactory.
			}

			ContactLocationMessageContent::ContactLocationMessageContent(double latitude, double longitude, double height, QString const& description) : ContactMessageContent(), latitude(latitude), longitude(longitude), height(height), description(description) {
				// Intentionally left empty.
			}

			ContactLocationMessageContent::~ContactLocationMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactLocationMessageContent::clone() const {
				return new ContactLocationMessageContent(latitude, longitude, height, description);
			}

			double ContactLocationMessageContent::getLatitude() const {
				return latitude;
			}

			double ContactLocationMessageContent::getLongitude() const {
				return longitude;
			}

			double ContactLocationMessageContent::getHeight() const {
				return height;
			}

			QString const& ContactLocationMessageContent::getDescription() const {
				return description;
			}

			openmittsu::utility::Location ContactLocationMessageContent::getLocation() const {
				return openmittsu::utility::Location(latitude, longitude, height, "", description);
			}

			MessageContent* ContactLocationMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_CONTACT_LOCATION, 1 + 1, payload);

				// Layout of remaining payload:
				// 51.719827,9.888894,0.000000\nSome Road 1234

				QByteArray const remainingPayload(payload.mid(1));

				int const positionOfNewLine = remainingPayload.indexOf('\n');
				if (positionOfNewLine == -1) {
					throw openmittsu::exceptions::ProtocolErrorException() << "Could not split payload of a ContactLocationMessageContent, it does not contain the 0x0A LF splitter.";
				}

				QString const positions = QString::fromUtf8(remainingPayload.left(positionOfNewLine));
				QString const descriptionText = QString::fromUtf8(remainingPayload.mid(positionOfNewLine + 1));

				QStringList splitPositions = positions.split(',', QString::SkipEmptyParts);
				if (splitPositions.size() != 3) {
					throw openmittsu::exceptions::ProtocolErrorException() << "Could not split payload of a ContactLocationMessageContent, it contains " << splitPositions.size() << " instead of 3 positions. Input String: " << positions.toStdString();
				}

				bool ok = false;

				double const lat = splitPositions.at(0).toDouble(&ok);
				if (!ok) {
					throw openmittsu::exceptions::ProtocolErrorException() << "Could not convert position 1 to double. Input String: " << positions.toStdString();
				}

				double const lon = splitPositions.at(1).toDouble(&ok);
				if (!ok) {
					throw openmittsu::exceptions::ProtocolErrorException() << "Could not convert position 2 to double. Input String: " << positions.toStdString();
				}

				double const hei = splitPositions.at(2).toDouble(&ok);
				if (!ok) {
					throw openmittsu::exceptions::ProtocolErrorException() << "Could not convert position 3 to double. Input String: " << positions.toStdString();
				}

				return new ContactLocationMessageContent(lat, lon, hei, descriptionText);
			}

			QByteArray ContactLocationMessageContent::toPacketPayload() const {
				QByteArray result(1, PROTO_MESSAGE_SIGNATURE_CONTACT_LOCATION);

				QString const positionString = QString("%1,%2,%3").arg(latitude, 0, 'f', 6).arg(longitude, 0, 'f', 6).arg(height, 0, 'f', 6);

				result.append(positionString.toUtf8());
				result.append('\n');
				result.append(description.toUtf8());

				return result;
			}

		}
	}
}
