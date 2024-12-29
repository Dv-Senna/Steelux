#pragma once

#include <cstdint>
#include <chrono>

#include "sl/core.hpp"
#include "sl/memory/allocatorTraits.hpp"


namespace sl::memory {
	template <typename T>
	class SL_CORE DebugAllocator final {
		public:
			using value_type = T;

			constexpr DebugAllocator() noexcept;
			constexpr ~DebugAllocator();

			constexpr T *allocate(std::size_t n) noexcept;
			constexpr void deallocate(T *ptr, std::size_t n) noexcept;

			constexpr bool operator==(const DebugAllocator<T> &) const noexcept {return true;}


		private:
			static std::size_t s_instanceCount;
			static std::size_t s_allocationCount;
			static float s_averageMsBetweenAllocation;
			static std::chrono::high_resolution_clock::time_point s_lastAllocation;
	};

	template <typename T>
	struct IsAllocatorStatefull<sl::memory::DebugAllocator<T>> {static constexpr bool value {false};};

	static_assert(sl::memory::IsAllocator<DebugAllocator<char>>);

} // namespace sl::memory

#include "sl/memory/debugAllocator.inl"
