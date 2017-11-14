#ifndef OPENMITTSU_BACKUP_FILEREADER_H_
#define OPENMITTSU_BACKUP_FILEREADER_H_

#include <QDir>
#include <QFile>
#include <QHash>
#include <QString>
#include <QTextStream>
#include <QVector>
#include <cstdint>

namespace openmittsu {
	namespace backup {

		template<typename T>
		class FileReader {
		public:
			FileReader(QDir const& path, QString const& filename);
			virtual ~FileReader();

			bool hasNext();
			T getNext();
		private:
			bool requireFileReadable();
			QHash<QString, int> parseOffsets(QString const& header) const;

			QDir const m_path;
			QString const m_filename;
			QFile m_inputFile;
			QTextStream m_inputStream;
			QHash<QString, int> m_headerOffsets;
		};

	}
}

#endif // OPENMITTSU_BACKUP_FILEREADER_H_
