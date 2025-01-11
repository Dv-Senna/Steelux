#pragma once

#include <fstream>
#include <vector>

#include "sl/core.hpp"


namespace sl::utils {
	SL_CORE auto readBinaryFile(std::ifstream &stream) noexcept -> std::vector<std::byte>;

} // namespace sl::utils
