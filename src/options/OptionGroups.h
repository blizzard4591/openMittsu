#ifndef OPENMITTSU_OPTIONS_OPTIONGROUPS_H_
#define OPENMITTSU_OPTIONS_OPTIONGROUPS_H_

#include <QHash>
#include <QMetaType>

namespace openmittsu {
	namespace options {
		enum class OptionGroups {
			GROUP_PRIVACY,
			GROUP_NOTIFICATIONS,
			GROUP_GENERAL,
			GROUP_INTERNAL
		};
	}
}

uint qHash(openmittsu::options::OptionGroups const& key, uint seed);

Q_DECLARE_METATYPE(openmittsu::options::OptionGroups)

#endif // OPENMITTSU_OPTIONS_OPTIONGROUPS_H_
