#pragma once

#include "sl/memory/stackAllocator.hpp"
#include "sl/utils/units.hpp"


namespace sl::memory {
	class SingleFrameAllocator {
		public:
			using value_type = sl::memory::StackAllocator::value_type;
			using pointer = sl::memory::StackAllocator::pointer;
			using const_pointer = sl::memory::StackAllocator::const_pointer;
			using size_type = sl::memory::StackAllocator::size_type;
			using difference_type = sl::memory::StackAllocator::difference_type;

			inline SingleFrameAllocator(sl::utils::Bytes size = 16_MiB) noexcept : m_stackAllocator {size} {}
			inline ~SingleFrameAllocator() = default;

			inline SingleFrameAllocator(const SingleFrameAllocator &) noexcept = delete;
			inline auto operator=(const SingleFrameAllocator &) noexcept -> SingleFrameAllocator& = delete;

			inline SingleFrameAllocator(SingleFrameAllocator &&) noexcept = default;
			inline auto operator=(SingleFrameAllocator &&) noexcept -> SingleFrameAllocator& = default;

			[[nodiscard]]
			inline auto allocate(size_type size, size_type alignment) noexcept -> pointer {return m_stackAllocator.allocate(size, alignment);}
			inline auto clear() noexcept -> void {m_stackAllocator.clear();}

		private:
			sl::memory::StackAllocator m_stackAllocator;
	};


	template <typename T>
	class SingleFrameAllocatorView {
		public:
			using value_type = T;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			using reference = value_type&;
			using const_reference = const value_type&;
			using size_type = SingleFrameAllocator::size_type;
			using difference_type = SingleFrameAllocator::difference_type;
			using is_always_equal = std::false_type;

			inline SingleFrameAllocatorView(SingleFrameAllocator &allocator) noexcept : m_allocator {&allocator} {}
			inline ~SingleFrameAllocatorView() = default;

			inline SingleFrameAllocatorView(const SingleFrameAllocatorView<T> &) noexcept = default;
			inline auto operator=(const SingleFrameAllocatorView<T> &) noexcept -> SingleFrameAllocatorView& = default;
			inline SingleFrameAllocatorView(SingleFrameAllocatorView<T> &&) noexcept = default;
			inline auto operator=(SingleFrameAllocatorView<T> &&) noexcept -> SingleFrameAllocatorView& = default;

			[[nodiscard]]
			inline auto allocate(size_type n) noexcept -> pointer {return reinterpret_cast<pointer> (m_allocator->allocate(sizeof(T) * n, alignof(T)));}
			inline auto deallocate(pointer, size_type) noexcept -> void {}
			inline auto clear() noexcept -> void {m_allocator->clear();}

			inline auto operator==(const SingleFrameAllocatorView<T> &view) const noexcept -> bool {return m_allocator == view.m_allocator;}

		private:
			SingleFrameAllocator *m_allocator;
	};

	static_assert(sl::memory::IsAllocator<SingleFrameAllocatorView<char>>);

	class SingleFrameMemoryResource final : public std::pmr::memory_resource {
		public:
			inline SingleFrameMemoryResource(SingleFrameAllocator &allocator) noexcept : m_allocator {&allocator} {}
			inline SingleFrameMemoryResource(const SingleFrameMemoryResource &) noexcept = default;
			inline auto operator=(const SingleFrameMemoryResource &) noexcept -> SingleFrameMemoryResource& = default;
			inline SingleFrameMemoryResource(SingleFrameMemoryResource &&) noexcept = default;
			inline auto operator=(SingleFrameMemoryResource &&) noexcept -> SingleFrameMemoryResource& = default;

		private:
			inline auto do_allocate(std::size_t bytes, std::size_t alignment) -> void* override {return m_allocator->allocate(bytes, alignment);}
			inline auto do_deallocate(void*, std::size_t, std::size_t) -> void override {}
			inline auto do_is_equal(const std::pmr::memory_resource &other) const noexcept -> bool override {
				const SingleFrameMemoryResource *resource {dynamic_cast<const SingleFrameMemoryResource*> (&other)};
				if (resource == nullptr)
					return false;
				return m_allocator == resource->m_allocator;
			}

			SingleFrameAllocator *m_allocator;
	};

} // namespace sl::memory
