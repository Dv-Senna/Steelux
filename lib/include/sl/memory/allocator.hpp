#pragma once

#include <memory>

#include "sl/memory/allocatorTraits.hpp"


namespace sl::memory {
	template <typename T>
	using DefaultAllocator = std::allocator<T>;

} // namespace sl::memory

