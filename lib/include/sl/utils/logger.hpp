#pragma once

#include "sl/core.hpp"
#include "sl/utils/enums.hpp"
#include "sl/utils/string.hpp"


namespace sl::utils {
	enum class LogSeverity : sl::utils::FlagEnumBit {
		eDebug = 0_feb,
		eInfo  = 1_feb,
		eWarn  = 2_feb,
		eError = 3_feb
	};

	class SL_CORE Logger {
		public:

	};

} // namespace sl::utils
