#pragma once

#include "sl/utils/enums.hpp"


namespace sl::utils {
	enum class Endianness : sl::utils::PackedEnumString {
		eLittle = "little"_pes,
		eBig = "big"_pes
	};

	constexpr auto getCompiletimeEndianness() noexcept -> Endianness;
	inline auto getRuntimeEndianness() noexcept -> Endianness;
} // namespace sl::utils

#include "sl/utils/endianness.inl"
