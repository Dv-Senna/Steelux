#pragma once

#include <cstddef>
#include <expected>
#include <memory>

#include "sl/memory/debugAllocator.hpp"


namespace sl::memory {
	class AllocatorView {
		public:
			constexpr AllocatorView() noexcept = default;
			constexpr virtual ~AllocatorView() = default;

			[[nodiscard]] virtual std::byte *allocate(std::size_t n) noexcept = 0;
			virtual void deallocate(std::byte *ptr, std::size_t n) noexcept = 0;

			constexpr virtual std::unique_ptr<AllocatorView> copy() noexcept = 0;
	};


	template <typename Alloc>
	class AllocatorViewFactory final : public sl::memory::AllocatorView {
		public:
			// use the default constructor for stateless allocator
			constexpr AllocatorViewFactory() noexcept;
			constexpr ~AllocatorViewFactory() override;

			// use this constructor for statefull allocator
			constexpr AllocatorViewFactory(Alloc &allocator) noexcept;

			[[nodiscard]] std::byte *allocate(std::size_t n) noexcept override;
			void deallocate(std::byte *ptr, std::size_t n) noexcept override;

			constexpr std::unique_ptr<AllocatorView> copy() noexcept override;

		private:
			Alloc *m_allocator;
	};


#ifndef NDEBUG
	template <typename T>
	using DefaultAllocator = sl::memory::DebugAllocator<T>;
#else
	template <typename T>
	using DefaultAllocator = std::allocator<T>;
#endif

} // namespace sl::memory

#include "sl/memory/allocator.inl"
