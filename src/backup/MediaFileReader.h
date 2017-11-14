#ifndef OPENMITTSU_BACKUP_MEDIAFILEREADER_H_
#define OPENMITTSU_BACKUP_MEDIAFILEREADER_H_

#include <QDir>
#include <QFile>
#include <QHash>
#include <QString>
#include <QTextStream>
#include <QVector>
#include <cstdint>

namespace openmittsu {
	namespace backup {

		class MediaFileReader {
		public:
			MediaFileReader(QDir const& path);
			virtual ~MediaFileReader();

			bool hasNext();
			T getNext();
		private:
			bool requireFileReadable();
			QHash<QString, int> parseOffsets(QString const& header) const;

			QDir const m_path;
			QStringList const m_files;
		};

	}
}

#endif // OPENMITTSU_BACKUP_FILEREADER_H_
