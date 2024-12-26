#pragma once

#include "sl/utils/endianness.hpp"


namespace sl::utils {
	constexpr Endianness getCompiletimeEndianness() {
		union {
			std::uint32_t i;
			char c[4];
		} value {0x01020304};
		if (value.c[0] == 0x01)
			return Endianness::eBig;
		return Endianness::eLittle;
	}


	Endianness getRuntimeEndianness() {
		static Endianness endianness {};
		static bool firstRun {true};
		if (!firstRun)
			return endianness;

		union {
			std::uint32_t i;
			char c[4];
		} value {0x01020304};

		endianness = value.c[0] == 0x01 ? Endianness::eBig : Endianness::eLittle;
		return endianness;
	}

} // namespace sl::utils
