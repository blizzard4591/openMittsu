#ifndef OPENMITTSU_BACKUP_SIMPLECSVLINESPLITTER_H_
#define OPENMITTSU_BACKUP_SIMPLECSVLINESPLITTER_H_

#include <QString>
#include <QHash>
#include <cstdint>

namespace openmittsu {
	namespace backup {

		class SimpleCsvLineSplitter {
		public:
			SimpleCsvLineSplitter(QStringList const& columns);
			virtual ~SimpleCsvLineSplitter();

			int getNumberOfColumns() const;
			QString const& getColumn(int columnIndex) const;

			static SimpleCsvLineSplitter split(int numberOfColumns, QString const& line);
		private:
			QStringList const m_columns;
		};

	}
}

#endif // OPENMITTSU_BACKUP_SIMPLECSVLINESPLITTER_H_
