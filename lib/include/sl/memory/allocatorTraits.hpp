#pragma once

#include <memory>


namespace sl::memory {
	template <typename Alloc>
	concept IsAllocator = requires (Alloc alloc, std::size_t n) {
		{*alloc.allocate(n)} -> std::same_as<typename Alloc::value_type&>;
		{alloc.deallocate(alloc.allocate(n), n)};
	}
		&& std::copy_constructible<Alloc>
		&& std::equality_comparable<Alloc>;


	template <sl::memory::IsAllocator Alloc>
	struct IsAllocatorStatefull {static constexpr bool value {true};};

	template <sl::memory::IsAllocator Alloc>
	constexpr bool IsAllocatorStatefull_v = IsAllocatorStatefull<Alloc>::value;


	template <typename T>
	struct IsAllocatorStatefull<std::allocator<T>> {static constexpr bool value {false};};
} // namespace sl::memory
