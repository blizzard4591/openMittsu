#include "messages/group/GroupLocationMessageContent.h"

#include "exceptions/ProtocolErrorException.h"
#include "messages/MessageContentRegistry.h"
#include "protocol/GroupId.h"
#include "protocol/ProtocolSpecs.h"

#include <QStringList>

// Register this MessageContent with the MessageContentRegistry
bool GroupLocationMessageContent::registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_GROUP_LOCATION, new TypedMessageContentFactory<GroupLocationMessageContent>());


GroupLocationMessageContent::GroupLocationMessageContent() : GroupMessageContent(GroupId(0, 0)), latitude(0.0), longitude(0.0), height(0.0), description() {
	// Only accessible and used by the MessageContentFactory.
}

GroupLocationMessageContent::GroupLocationMessageContent(GroupId const& groupId, double latitude, double longitude, double height, QString const& description) : GroupMessageContent(groupId), latitude(latitude), longitude(longitude), height(height), description(description) {
	// Intentionally left empty.
}

GroupLocationMessageContent::~GroupLocationMessageContent() {
	// Intentionally left empty.
}

GroupMessageContent* GroupLocationMessageContent::clone() const {
	return new GroupLocationMessageContent(getGroupId(), latitude, longitude, height, description);
}

double GroupLocationMessageContent::getLatitude() const {
	return latitude;
}

double GroupLocationMessageContent::getLongitude() const {
	return longitude;
}

double GroupLocationMessageContent::getHeight() const {
	return height;
}

QString const& GroupLocationMessageContent::getDescription() const {
	return description;
}

MessageContent* GroupLocationMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
	verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_GROUP_LOCATION, 1 + GroupId::getSizeOfGroupIdInBytes() + 1, payload);

	GroupId const groupId(GroupId::fromData(payload.mid(1, GroupId::getSizeOfGroupIdInBytes())));

	// Layout of remaining payload:
	// 51.719827,9.888894,0.000000\nSome Road 1234

	QByteArray const remainingPayload(payload.mid(1 + GroupId::getSizeOfGroupIdInBytes()));

	int const positionOfNewLine = remainingPayload.indexOf('\n');
	if (positionOfNewLine == -1) {
		throw ProtocolErrorException() << "Could not split payload of a GroupLocationMessage, it does not contain the 0x0A LF splitter.";
	}

	QString const positions = QString::fromUtf8(remainingPayload.left(positionOfNewLine));
	QString const descriptionText = QString::fromUtf8(remainingPayload.mid(positionOfNewLine + 1));

	QStringList splitPositions = positions.split(',', QString::SkipEmptyParts);
	if (splitPositions.size() != 3) {
		throw ProtocolErrorException() << "Could not split payload of a GroupLocationMessage, it contains " << splitPositions.size() << " instead of 3 positions. Input String: " << positions.toStdString();
	}

	bool ok = false;

	double const lat = splitPositions.at(0).toDouble(&ok);
	if (!ok) {
		throw ProtocolErrorException() << "Could not convert position 1 to double. Input String: " << positions.toStdString();
	}

	double const lon = splitPositions.at(1).toDouble(&ok);
	if (!ok) {
		throw ProtocolErrorException() << "Could not convert position 2 to double. Input String: " << positions.toStdString();
	}

	double const hei = splitPositions.at(2).toDouble(&ok);
	if (!ok) {
		throw ProtocolErrorException() << "Could not convert position 3 to double. Input String: " << positions.toStdString();
	}

	return new GroupLocationMessageContent(groupId, lat, lon, hei, descriptionText);
}

QByteArray GroupLocationMessageContent::toPacketPayload() const {
	QByteArray result(1, PROTO_MESSAGE_SIGNATURE_GROUP_LOCATION);
	result.append(getGroupId().getGroupIdAsByteArray());
	
	QString const positionString = QString("%1,%2,%3").arg(latitude, 0, 'f', 6).arg(longitude, 0, 'f', 6).arg(height, 0, 'f', 6);

	result.append(positionString.toUtf8());
	result.append('\n');
	result.append(description.toUtf8());

	return result;
}
