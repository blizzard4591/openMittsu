#ifndef OPENMITTSU_DATABASE_MEDIAFILEITEM_H_
#define OPENMITTSU_DATABASE_MEDIAFILEITEM_H_

#include <QByteArray>

class QPixmap;

namespace openmittsu {
	namespace database {
		
		class MediaFileItem {
		public:
			enum class ItemStatus {
				AVAILABLE,
				UNAVAILABLE_NOT_IN_DATABASE,
				UNAVAILABLE_EXTERNAL_FILE_DELETED,
				UNAVAILABLE_FILE_CORRUPTED,
				UNAVAILABLE_DECRYPTION_FAILED,
				UNAVAILABLE_CRYPTO_ERROR
			};

			explicit MediaFileItem(QByteArray const& data);
			explicit MediaFileItem(ItemStatus reasonOfUnavailablity);
			virtual ~MediaFileItem();

			QByteArray const& getData() const;
			bool isAvailable() const;
			QPixmap getPixmapWithErrorMessage(int width, int height) const;
		private:
			QByteArray const m_data;
			ItemStatus const m_status;

		};

	}
}

#endif // OPENMITTSU_DATABASE_MEDIAFILEITEM_H_
