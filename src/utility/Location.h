#ifndef OPENMITTSU_UTILITY_LOCATION_H_
#define OPENMITTSU_UTILITY_LOCATION_H_

#include <QtGlobal>
#include <QMetaType>
#include <QString>
#include <QVariant>

namespace QtPrivate {
	template <typename T> struct QVariantValueHelper;
}

namespace openmittsu {
	namespace utility {

		class Location {
		public:
			// For QVariant
			Location();

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

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
			friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<Location, true>;
#else
			friend class QVariant;
#endif
			friend struct QtPrivate::QVariantValueHelper<Location>;
		private:
			double m_latitude;
			double m_longitude;
			double m_height;
			QString m_address;
			QString m_description;
		};

	}
}

Q_DECLARE_METATYPE(openmittsu::utility::Location)


#endif // OPENMITTSU_UTILITY_LOCATION_H_
