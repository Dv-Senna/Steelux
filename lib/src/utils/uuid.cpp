#include "sl/utils/uuid.hpp"


namespace sl::utils {
	auto generateUUID() noexcept -> std::uintmax_t {
		static std::uintmax_t last {0};
		return ++last;
	}

} // namespace sl::utils
