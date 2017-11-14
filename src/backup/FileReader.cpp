#include "src/backup/FileReader.h"

#include <QFile>
#include <QTextStream>
#include <QString>

#include "src/utility/Logging.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/exceptions/InvalidInputException.h"
#include "src/exceptions/InsufficientInputException.h"

#include "src/backup/SimpleCsvLineSplitter.h"

#include "src/backup/ContactBackupObject.h"
#include "src/backup/GroupBackupObject.h"
#include "src/backup/ContactMessageBackupObject.h"
#include "src/backup/GroupMessageBackupObject.h"

namespace openmittsu {
	namespace backup {

		template<typename T>
		FileReader<T>::FileReader(QDir const& path, QString const& filename) : m_path(path), m_filename(filename), m_inputFile(m_path.filePath(m_filename)), m_inputStream() {
			if (!requireFileReadable()) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Could not parse file \"" << m_filename.toStdString() << "\", it is not readable.";
			}

			m_inputStream.setDevice(&m_inputFile);
			m_inputStream.setCodec("UTF-8");

			// parse header
			m_headerOffsets = parseOffsets(m_inputStream.readLine());
		}

		template<typename T>
		FileReader<T>::~FileReader() {
			m_inputFile.close();
		}

		template<typename T>
		bool FileReader<T>::hasNext() {
			return !m_inputStream.atEnd();
		}

		template<typename T>
		T FileReader<T>::getNext() {
			QString line;
			while (true) {
				if (!m_inputStream.atEnd()) {
					line.append(m_inputStream.readLine());

					try {
						SimpleCsvLineSplitter const splittedLines = SimpleCsvLineSplitter::split(m_headerOffsets.size(), line);
						return T::fromBackupMatch(m_filename, m_headerOffsets, splittedLines);
					} catch (openmittsu::exceptions::InsufficientInputException&) {
						continue;
					}
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "There is no next, yet getNext() was called!";
				}
			}
		}

		template<typename T>
		bool FileReader<T>::requireFileReadable() {
			if (!m_path.exists(m_filename)) {
				LOGGER()->warn("Can not import data, \"{}\" file does not exist in Backup.", m_filename.toStdString());
				return false;
			}

			m_inputFile.setFileName(m_path.filePath(m_filename));
			if (!m_inputFile.open(QIODevice::ReadOnly)) {
				LOGGER()->warn("Can not import data, \"{}\" file can not be opened for reading.", m_filename.toStdString());
				return false;
			}

			return true;
		}

		template<typename T>
		QHash<QString, int> FileReader<T>::parseOffsets(QString const& header) const {
			QHash<QString, int> result;
			QStringList const parts = header.split(',', QString::SkipEmptyParts);
			for (int i = 0; i < parts.size(); ++i) {
				int const size = parts.at(i).size();
				result.insert(parts.at(i).mid(1, size - 2), i);
			}
			return result;
		}


		template class FileReader<ContactBackupObject>;
		template class FileReader<GroupBackupObject>;
		template class FileReader<ContactMessageBackupObject>;
		template class FileReader<GroupMessageBackupObject>;
	}
}

