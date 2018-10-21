#ifndef OPENMITTSU_DATABASE_MEDIAFILEITEM_H_
#define OPENMITTSU_DATABASE_MEDIAFILEITEM_H_

#include <QByteArray>
#include "src/database/MediaFileType.h"

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

			MediaFileItem() = default;
			explicit MediaFileItem(QByteArray const& data, MediaFileType const& type);
			explicit MediaFileItem(ItemStatus reasonOfUnavailablity, MediaFileType const& type);

			virtual ~MediaFileItem();

			MediaFileType getFileType() const;
			QByteArray const& getData() const;
			bool isAvailable() const;
			QPixmap getPixmapWithErrorMessage(int width, int height) const;
		private:
			QByteArray m_data;
			ItemStatus m_status;
			MediaFileType m_type;
		};

	}
}

#endif // OPENMITTSU_DATABASE_MEDIAFILEITEM_H_
