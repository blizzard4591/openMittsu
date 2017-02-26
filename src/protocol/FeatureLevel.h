#ifndef OPENMITTSU_PROTOCOL_FEATURELEVEL_H_
#define OPENMITTSU_PROTOCOL_FEATURELEVEL_H_

enum class FeatureLevel : int {
	LEVEL_UNKNOW = -2,
	LEVEL_INVALID = -1,
	LEVEL_0 = 0, // Text, Pictures, Videos
	LEVEL_1 = 1, // Audio, Groups
	LEVEL_2 = 2, // Ballots
	LEVEL_3 = 3,  // Files
};

class FeatureLevelHelper {
public:
	static FeatureLevel intToFeatureLevel(int featureLevel);
	static int featureLevelToInt(FeatureLevel const& featureLevel);
	static bool lessThan(FeatureLevel const& left, FeatureLevel const& right);

	static FeatureLevel latestSupported();
};

#endif // OPENMITTSU_PROTOCOL_FEATURELEVEL_H_
