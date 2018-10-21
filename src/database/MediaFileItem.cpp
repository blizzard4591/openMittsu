#include "src/database/MediaFileItem.h"

#include <QPainter>
#include <QPixmap>

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace database {

		MediaFileItem::MediaFileItem(QByteArray const& data, MediaFileType const& type) : m_data(data), m_status(ItemStatus::AVAILABLE), m_type(type) {
			//
		}

		MediaFileItem::MediaFileItem(ItemStatus reasonOfUnavailablity, MediaFileType const& type) : m_data(), m_status(reasonOfUnavailablity), m_type(type) {
			//
		}

		MediaFileItem::~MediaFileItem() {
			//
		}

		QByteArray const& MediaFileItem::getData() const {
			if (m_status == ItemStatus::AVAILABLE) {
				return m_data;
			}
			throw openmittsu::exceptions::InternalErrorException() << "MediaFileItem::getData() was accessed, but this item is not available.";
		}

		bool MediaFileItem::isAvailable() const {
			return m_status == ItemStatus::AVAILABLE;
		}

		MediaFileType MediaFileItem::getFileType() const {
			return m_type;
		}

		QPixmap MediaFileItem::getPixmapWithErrorMessage(int width, int height) const {
			QPixmap result(width, height);

			result.fill(Qt::white);
			{
				QPainter painter(&result);
				painter.setPen(Qt::black);
				painter.setFont(QFont("Arial", 12));

				switch (m_status) {
					case ItemStatus::AVAILABLE:
						painter.drawText(result.rect(), Qt::AlignCenter, "No error occurred.");
						break;
					case ItemStatus::UNAVAILABLE_NOT_IN_DATABASE:
						painter.drawText(result.rect(), Qt::AlignCenter, "Error: Referenced media item is not listed in database.");
						break;
					case ItemStatus::UNAVAILABLE_EXTERNAL_FILE_DELETED:
						painter.drawText(result.rect(), Qt::AlignCenter, "Error: Referenced media item was deleted from external storage.");
						break;
					case ItemStatus::UNAVAILABLE_FILE_CORRUPTED:
						painter.drawText(result.rect(), Qt::AlignCenter, "Error: Referenced media item on external storage is corrupted.");
						break;
					case ItemStatus::UNAVAILABLE_DECRYPTION_FAILED:
						painter.drawText(result.rect(), Qt::AlignCenter, "Error: Referenced media item could not be decrypted.");
						break;
					case ItemStatus::UNAVAILABLE_CRYPTO_ERROR:
						painter.drawText(result.rect(), Qt::AlignCenter, "Error: Cryptographic error while fetching referenced media item.");
						break;
					default:
						painter.drawText(result.rect(), Qt::AlignCenter, "An unexpected error occurred.");
						break;
				}
			}

			return result;
		}
	}
}
