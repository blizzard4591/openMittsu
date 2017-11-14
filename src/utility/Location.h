#ifndef OPENMITTSU_UTILITY_LOCATION_H_
#define OPENMITTSU_UTILITY_LOCATION_H_

#include <QString>

namespace openmittsu {
	namespace utility {

		class Location {
		public:
			Location(double latitude, double longitude, double height, QString const& address, QString const& description);

			QString toDatabaseString() const;
			double getLatitude() const;
			double getLongitude() const;
			double getHeight() const;
			QString const& getAddress() const;
			QString const& getDescription() const;

			static Location fromDatabaseString(QString const& databaseLocationString);

			QString toQString() const;
			std::string toString() const;

			bool operator ==(Location const& other) const;
			bool operator !=(Location const& other) const;
		private:
			const double m_latitude;
			const double m_longitude;
			const double m_height;
			QString const m_address;
			QString const m_description;
		};

	}
}

#endif // OPENMITTSU_UTILITY_LOCATION_H_
