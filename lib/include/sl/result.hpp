#pragma once

#include <cstdint>



namespace sl {
	enum class Result : std::uint64_t {
		eSuccess = 0,
		eFailure,
		eAllocationFailure,
		eFileFailure
	};

} // namespace sl
