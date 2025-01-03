#pragma once

#include "sl/memory/debugAllocator.hpp"

#include <cstdlib>
#include <print>

#ifdef __GNUC__
	#include <cxxabi.h>
#endif


namespace sl::memory {
	template <typename T>
	constexpr DebugAllocator<T>::DebugAllocator() noexcept {
		if (!std::is_constant_evaluated())
			++s_instanceCount;
	}


	template <typename T>
	constexpr DebugAllocator<T>::~DebugAllocator() {
		if (std::is_constant_evaluated())
			return;
		--s_instanceCount;
		if (s_instanceCount != 0)
			return;

		std::string typeName {typeid(T).name()};

	#ifdef __GNUC__
		int status {};
		char *demangledName {abi::__cxa_demangle(typeName.c_str(), nullptr, nullptr, &status)};
		if (status == 0) {
			typeName = demangledName;
			std::free(demangledName);
		}
	#endif

		sl::memory::__DebugAllocator_print(typeName, s_allocationCount, s_averageMsBetweenAllocation);
	}


	template <typename T>
	[[nodiscard]]
	constexpr auto DebugAllocator<T>::allocate(std::size_t n) noexcept -> T* {
		if (std::is_constant_evaluated())
			return new T[n];

		if (s_allocationCount != 0) {
			s_averageMsBetweenAllocation *= static_cast<float> (s_allocationCount);
			s_averageMsBetweenAllocation += std::chrono::duration_cast<std::chrono::duration<float, std::milli>> (std::chrono::high_resolution_clock::now() - s_lastAllocation).count();
			s_averageMsBetweenAllocation /= static_cast<float> (s_allocationCount + 1);
		}

		++s_allocationCount;
		s_lastAllocation = std::chrono::high_resolution_clock::now();
		return new T[n];
	}


	template <typename T>
	constexpr auto DebugAllocator<T>::deallocate(T *ptr, std::size_t) noexcept -> void {
		delete[] ptr;
	}


	template <typename T> std::size_t DebugAllocator<T>::s_instanceCount {0};
	template <typename T> std::size_t DebugAllocator<T>::s_allocationCount {0};
	template <typename T> float DebugAllocator<T>::s_averageMsBetweenAllocation {0};
	template <typename T> std::chrono::high_resolution_clock::time_point DebugAllocator<T>::s_lastAllocation {};

} // namespace sl::memory
