#include "src/protocol/FeatureLevel.h"

namespace openmittsu {
	namespace protocol {

		FeatureLevel FeatureLevelHelper::fromInt(int featureLevel) {
			switch (featureLevel) {
				case 0:
				case 1:
				case 2:
				case 3:
				case -1:
				case -2:
					return static_cast<FeatureLevel>(featureLevel);
				default:
					return FeatureLevel::LEVEL_INVALID;
			}
		}

		int FeatureLevelHelper::toInt(FeatureLevel const& featureLevel) {
			switch (featureLevel) {
				case FeatureLevel::LEVEL_0:
				case FeatureLevel::LEVEL_1:
				case FeatureLevel::LEVEL_2:
				case FeatureLevel::LEVEL_3:
				case FeatureLevel::LEVEL_UNKNOW:
					return static_cast<int>(featureLevel);
				default:
					return static_cast<int>(FeatureLevel::LEVEL_INVALID);
			}
		}

		bool FeatureLevelHelper::lessThan(FeatureLevel const& left, FeatureLevel const& right) {
			if (left == FeatureLevel::LEVEL_INVALID || left == FeatureLevel::LEVEL_UNKNOW) {
				return false;
			} else if (right == FeatureLevel::LEVEL_INVALID || right == FeatureLevel::LEVEL_UNKNOW) {
				return false;
			} else {
				return static_cast<int>(left) < static_cast<int>(right);
			}
		}

		FeatureLevel FeatureLevelHelper::latestSupported() {
			return FeatureLevel::LEVEL_3;
		}

	}
}
