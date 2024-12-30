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
	struct IsAllocatorStatefull;

	template <sl::memory::IsAllocator Alloc>
	requires std::same_as<typename std::allocator_traits<Alloc>::is_always_equal, std::true_type>
	struct IsAllocatorStatefull<Alloc> {static constexpr bool value {false};};

	template <sl::memory::IsAllocator Alloc>
	requires std::same_as<typename std::allocator_traits<Alloc>::is_always_equal, std::false_type>
	struct IsAllocatorStatefull<Alloc> {static constexpr bool value {true};};

	template <sl::memory::IsAllocator Alloc>
	constexpr bool IsAllocatorStatefull_v = IsAllocatorStatefull<Alloc>::value;

} // namespace sl::memory
