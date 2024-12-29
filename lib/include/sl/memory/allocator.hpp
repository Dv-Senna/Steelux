#pragma once

#include <expected>
#include <memory>

#include "sl/memory/allocatorTraits.hpp"
#include "sl/memory/debugAllocator.hpp"


namespace sl::memory {
#ifndef NDEBUG
	template <typename T>
	using DefaultAllocator = sl::memory::DebugAllocator<T>;
#else
	template <typename T>
	using DefaultAllocator = std::allocator<T>;
#endif

} // namespace sl::memory

