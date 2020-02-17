#ifndef OPENMITTSU_UTILITY_STRINGLIST_H_
#define OPENMITTSU_UTILITY_STRINGLIST_H_

#include <QString>
#include <QStringList>

#include <cstdint>
#include <utility>

namespace openmittsu {
	namespace utility {

		class StringList {
		public:
			StringList(QString const& list);
			StringList(QStringList const& list);
			virtual ~StringList();

			int size() const;
			QString toString() const;

			static QStringList split(QString const& s);
		private:
			QStringList const m_list;
		};

	}
}

#endif // OPENMITTSU_UTILITY_STRINGLIST_H_