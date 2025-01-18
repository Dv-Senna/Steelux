#pragma once

#include <cstddef>
#include <optional>

#include <vulkan/vulkan.h>

#include "sl/utils/units.hpp"


namespace sl::memory::gpu {
	template <typename T>
	concept IsAllocator = std::copyable<T>
		&& std::is_default_constructible_v<T>
		&& std::is_void_v<typename T::GpuAllocatorTag>
		&& requires(
			T allocator,
			const T constAllocator,
			sl::utils::Bytes size,
			sl::utils::Bytes alignment,
			std::size_t position
	) {
		{allocator.allocate(size, alignment)} -> std::same_as<std::optional<std::size_t>>;
		{allocator.deallocate(position, size)} -> std::same_as<void>;
		{constAllocator.getMemory()} -> std::same_as<VkDeviceMemory>;
	};

} // namespace sl::memory::gpu
