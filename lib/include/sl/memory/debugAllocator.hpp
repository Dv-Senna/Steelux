#pragma once

#include <cstdint>
#include <chrono>

#include "sl/core.hpp"


namespace sl::memory {
	template <typename T>
	class SL_CORE DebugAllocator final {
		public:
			using value_type = T;

			DebugAllocator() noexcept;
			~DebugAllocator();

			T *allocate(std::size_t n) noexcept;
			void deallocate(T *ptr, std::size_t n) noexcept;


		private:
			static std::size_t s_instanceCount;
			static std::size_t s_allocationCount;
			static float s_averageMsBetweenAllocation;
			static std::chrono::high_resolution_clock::time_point s_lastAllocation;
	};

} // namespace sl::memory

#include "sl/memory/debugAllocator.inl"
