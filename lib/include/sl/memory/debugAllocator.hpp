#pragma once

#include <cstdint>
#include <chrono>
#include <string>

#include "sl/core.hpp"
#include "sl/memory/allocatorTraits.hpp"


namespace sl::memory {
	template <typename T>
	class SL_CORE DebugAllocator final {
		public:
			using value_type = T;
			using is_always_equal = std::true_type;

			constexpr DebugAllocator() noexcept;
			constexpr ~DebugAllocator();

			[[nodiscard]]
			constexpr auto allocate(std::size_t n) noexcept -> T*;
			constexpr auto deallocate(T *ptr, std::size_t n) noexcept -> void;

			constexpr auto operator==(const DebugAllocator<T> &) const noexcept -> bool {return true;}

			inline auto clear() noexcept -> void;


		private:
			static std::size_t s_instanceCount;
			static std::size_t s_allocationCount;
			static float s_averageMsBetweenAllocation;
			static std::chrono::high_resolution_clock::time_point s_lastAllocation;
	};

	static_assert(sl::memory::IsAllocator<DebugAllocator<char>>);

	auto __DebugAllocator_print(const std::string &typeName, std::size_t allocationCount, float averageMsBetweenAllocation) -> void;

} // namespace sl::memory

#include "sl/memory/debugAllocator.inl"
