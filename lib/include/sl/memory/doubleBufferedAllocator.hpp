#pragma once

#include "sl/memory/stackAllocator.hpp"
#include "sl/utils/units.hpp"


namespace sl::memory {
	class DoubleBufferedAllocator {
		public:
			using value_type = sl::memory::StackAllocator::value_type;
			using pointer = sl::memory::StackAllocator::pointer;
			using const_pointer = sl::memory::StackAllocator::const_pointer;
			using size_type = sl::memory::StackAllocator::size_type;
			using difference_type = sl::memory::StackAllocator::difference_type;

			inline DoubleBufferedAllocator(sl::utils::Bytes size = 16_MiB) noexcept :
				m_currentStack {0},
				m_stackAllocators {sl::memory::StackAllocator(size), sl::memory::StackAllocator(size)} {}
			inline ~DoubleBufferedAllocator() = default;

			inline DoubleBufferedAllocator(const DoubleBufferedAllocator &) noexcept = delete;
			inline auto operator=(const DoubleBufferedAllocator &) noexcept -> DoubleBufferedAllocator& = delete;

			inline DoubleBufferedAllocator(DoubleBufferedAllocator &&) noexcept = default;
			inline auto operator=(DoubleBufferedAllocator &&) noexcept -> DoubleBufferedAllocator& = default;

			[[nodiscard]]
			inline auto allocate(size_type size, size_type alignment) noexcept -> pointer {return m_stackAllocators[m_currentStack].allocate(size, alignment);}
			inline auto clearCurrentBuffer() noexcept -> void {m_stackAllocators[m_currentStack].clear();}

			inline auto swapBuffer() noexcept -> void {m_currentStack = 1 - m_currentStack;}


		private:
			size_type m_currentStack;
			sl::memory::StackAllocator m_stackAllocators[2];
	};


	template <typename T>
	class DoubleBufferedAllocatorView {
		public:
			using value_type = T;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			using reference = value_type&;
			using const_reference = const value_type&;
			using size_type = DoubleBufferedAllocator::size_type;
			using difference_type = DoubleBufferedAllocator::difference_type;
			using is_always_equal = std::false_type;

			inline DoubleBufferedAllocatorView(DoubleBufferedAllocator &allocator) noexcept : m_allocator {&allocator} {}
			inline ~DoubleBufferedAllocatorView() = default;

			inline DoubleBufferedAllocatorView(const DoubleBufferedAllocatorView<T> &) noexcept = default;
			inline auto operator=(const DoubleBufferedAllocatorView<T> &) noexcept -> DoubleBufferedAllocatorView& = default;
			inline DoubleBufferedAllocatorView(DoubleBufferedAllocatorView<T> &&) noexcept = default;
			inline auto operator=(DoubleBufferedAllocatorView<T> &&) noexcept -> DoubleBufferedAllocatorView& = default;

			[[nodiscard]]
			inline auto allocate(size_type n) noexcept -> pointer {return reinterpret_cast<pointer> (m_allocator->allocate(sizeof(T) * n, alignof(T)));}
			inline auto deallocate(pointer, size_type) noexcept -> void {}
			inline auto clearCurrentBuffer() noexcept -> void {m_allocator->clearCurrentBuffer();}
			inline auto swapBuffer() noexcept -> void {m_allocator->swapBuffer();}

			inline auto operator==(const DoubleBufferedAllocatorView<T> &view) const noexcept -> bool {return m_allocator == view.m_allocator;}

		private:
			DoubleBufferedAllocator *m_allocator;
	};

	static_assert(sl::memory::IsAllocator<DoubleBufferedAllocatorView<char>>);

} // namespace sl::memory
