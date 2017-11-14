#include "src/utility/Location.h"

#include "src/exceptions/InternalErrorException.h"

#include <QJsonDocument>
#include <QJsonArray>

namespace openmittsu {
	namespace utility {

		Location::Location(double latitude, double longitude, double height, QString const& address, QString const& description) : m_latitude(latitude), m_longitude(longitude), m_height(height), m_address(address), m_description(description) {
			//
		}

		QString Location::toDatabaseString() const {
			QJsonArray jsonArray;
			jsonArray.push_back(m_latitude);
			jsonArray.push_back(m_longitude);
			jsonArray.push_back(m_height);
			jsonArray.push_back(m_address);
			jsonArray.push_back(m_description);
			QJsonDocument jsonDocument;
			jsonDocument.setArray(jsonArray);

			//QString const locationData = QStringLiteral("[%1,%2,0.0,\"%3\",\"%4\"]").arg(m_latitude).arg(m_longitude).arg(m_description).arg(QStringLiteral(""));

			return QString::fromUtf8(jsonDocument.toJson(QJsonDocument::JsonFormat::Compact));
		}

		QString Location::toQString() const {
			return QStringLiteral("Latitude: %1, Longitude: %2, Height: %3, Address: %4, Desciption: %5").arg(m_latitude).arg(m_longitude).arg(m_height).arg(m_address).arg(m_description);
		}

		std::string Location::toString() const {
			return toQString().toStdString();
		}

		double Location::getLatitude() const {
			return m_latitude;
		}

		double Location::getLongitude() const {
			return m_longitude;
		}

		double Location::getHeight() const {
			return m_height;
		}

		QString const& Location::getDescription() const {
			return m_description;
		}

		QString const& Location::getAddress() const {
			return m_address;
		}

		Location Location::fromDatabaseString(QString const& databaseLocationString) {
			QJsonParseError parseError;
			QJsonDocument jsonDocument = QJsonDocument::fromJson(databaseLocationString.toUtf8(), &parseError);
			if (jsonDocument.isNull()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not parse location string \"" << databaseLocationString.toStdString() << "\" into a JSON structure. Parser error: " << parseError.errorString().toStdString();
			} else if (!jsonDocument.isArray()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not parse location string \"" << databaseLocationString.toStdString() << "\" into a JSON structure. Contained object is not an array.";
			} else {
				QJsonArray jsonArray = jsonDocument.array();
				int const neededSize = 5;
				if (jsonArray.size() != neededSize) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not parse location string \"" << databaseLocationString.toStdString() << "\" into a JSON structure. Contained array has " << jsonArray.size() << " entries instead of " << neededSize << ".";
				} else {
					QJsonValue valueLat = jsonArray.at(0);
					QJsonValue valueLon = jsonArray.at(1);
					QJsonValue valueHei = jsonArray.at(2);
					QJsonValue valueAdr = jsonArray.at(3);
					QJsonValue valueDes = jsonArray.at(4);

					if (!valueLat.isDouble() || !valueLon.isDouble() || !valueHei.isDouble() || !valueAdr.isString() || !valueDes.isString()) {
						throw openmittsu::exceptions::InternalErrorException() << "Could not parse location string \"" << databaseLocationString.toStdString() << "\" into a JSON structure. Contained array has invalid types.";
					} else {
						return Location(valueLat.toDouble(), valueLon.toDouble(), valueHei.toDouble(), valueAdr.toString(), valueDes.toString());
					}
				}
			}
		}

		bool Location::operator ==(Location const& other) const {
			return (m_latitude == other.m_latitude) && (m_longitude == other.m_longitude) && (m_height == other.m_height) && (m_address == other.m_address) && (m_description == other.m_description);
		}

		bool Location::operator !=(Location const& other) const {
			return !(*this == other);
		}

	}
}
