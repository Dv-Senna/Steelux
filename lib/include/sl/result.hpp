#pragma once

#include "sl/utils/enums.hpp"



namespace sl {
	using namespace sl::utils::literals;

	enum class Result : sl::utils::PackedEnumString {
		eSuccess = "success"_pes,
		eFailure = "failure"_pes,
		eAllocationFailure = "allocation"_pes,
		eFileFailure = "file"_pes,
		eUnknown = "unknown"_pes
	};

} // namespace sl
