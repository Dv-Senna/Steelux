#pragma once

#include "sl/core.hpp"
#include "sl/memory/allocatorTraits.hpp"
#include "sl/utils/units.hpp"


namespace sl::memory {
	using namespace sl::utils::literals;

	class DoubleStackAllocator;


	class SL_CORE DoubleStackAllocatorMarker {
		friend class DoubleStackAllocator;

		public:
			using value_type = std::byte;
			using pointer = value_type*;

			constexpr DoubleStackAllocatorMarker() noexcept = default;
			constexpr ~DoubleStackAllocatorMarker() = default;
			constexpr DoubleStackAllocatorMarker(const DoubleStackAllocatorMarker &) noexcept = default;
			constexpr auto operator=(const DoubleStackAllocatorMarker &) noexcept -> DoubleStackAllocatorMarker& = default;
			constexpr DoubleStackAllocatorMarker(DoubleStackAllocatorMarker &&) noexcept = default;
			constexpr auto operator=(DoubleStackAllocatorMarker &&) noexcept -> DoubleStackAllocatorMarker& = default;


		protected:
			constexpr DoubleStackAllocatorMarker(pointer marker, bool isFromTopStack) noexcept : m_marker {marker}, m_isFromTopStack {isFromTopStack} {}

			pointer m_marker;
			bool m_isFromTopStack;
	};


	class SL_CORE DoubleStackAllocator {
		public:
			using value_type = std::byte;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;
			using Marker = DoubleStackAllocatorMarker;

			DoubleStackAllocator(sl::utils::Bytes size = 16_MiB) noexcept;
			~DoubleStackAllocator();

			DoubleStackAllocator(const DoubleStackAllocator &) noexcept = delete;
			auto operator=(const DoubleStackAllocator &) noexcept -> DoubleStackAllocator& = delete;

			DoubleStackAllocator(DoubleStackAllocator &&allocator) noexcept;
			auto operator=(DoubleStackAllocator &&allocator) noexcept -> DoubleStackAllocator&;

			[[nodiscard]]
			auto allocate(size_type size, size_type alignment) noexcept -> pointer;

			[[nodiscard]]
			auto mark() const noexcept -> Marker;
			auto deallocate(Marker marker) noexcept -> void;

			auto swapActiveStack() noexcept -> void;


		private:
			sl::utils::Bytes m_stackSize;
			pointer m_stackBase;
			pointer m_topStackTop;
			pointer m_bottomStackTop;
			bool m_isTopStackActive;
	};


	template <typename T>
	class DoubleStackAllocatorView {
		public:
			using value_type = T;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			using reference = value_type&;
			using const_reference = const value_type&;
			using size_type = DoubleStackAllocator::size_type;
			using difference_type = DoubleStackAllocator::difference_type;
			using is_always_equal = std::false_type;

			constexpr DoubleStackAllocatorView(DoubleStackAllocator &allocator) : m_allocator {&allocator} {}
			constexpr ~DoubleStackAllocatorView() = default;

			constexpr DoubleStackAllocatorView(const DoubleStackAllocatorView<T> &) noexcept = default;
			constexpr auto operator=(const DoubleStackAllocatorView<T> &) noexcept -> DoubleStackAllocatorView<T>& = default;
			constexpr DoubleStackAllocatorView(DoubleStackAllocatorView<T> &&) noexcept = default;
			constexpr auto operator=(DoubleStackAllocatorView<T> &&) noexcept -> DoubleStackAllocatorView<T>& = default;

			[[nodiscard]]
			inline auto allocate(size_type n) const noexcept -> pointer {return reinterpret_cast<pointer> (m_allocator->allocate(sizeof(T) * n, alignof(T)));}
			inline auto deallocate(pointer, size_type) const noexcept -> void {}

			[[nodiscard]]
			inline auto mark() const noexcept -> DoubleStackAllocator::Marker {return m_allocator->mark();}
			inline auto deallocate(DoubleStackAllocator::Marker marker) noexcept -> void {m_allocator->deallocate(marker);}

			inline auto swapActiveStack() noexcept -> void {m_allocator->swapActiveStack();}

			constexpr auto operator==(const DoubleStackAllocatorView<T> &view) const noexcept -> bool {return m_allocator == view.m_allocator;}

		private:
			DoubleStackAllocator *m_allocator;
	};


	static_assert(sl::memory::IsAllocator<DoubleStackAllocatorView<char>>);

	class DoubleStackMemoryResource final : public std::pmr::memory_resource {
		public:
			inline DoubleStackMemoryResource(DoubleStackAllocator &allocator) noexcept : m_allocator {&allocator} {}
			inline DoubleStackMemoryResource(const DoubleStackMemoryResource &) noexcept = default;
			inline auto operator=(const DoubleStackMemoryResource &) noexcept -> DoubleStackMemoryResource& = default;
			inline DoubleStackMemoryResource(DoubleStackMemoryResource &&) noexcept = default;
			inline auto operator=(DoubleStackMemoryResource &&) noexcept -> DoubleStackMemoryResource& = default;

		private:
			inline auto do_allocate(std::size_t bytes, std::size_t alignment) -> void* override {return m_allocator->allocate(bytes, alignment);}
			inline auto do_deallocate(void*, std::size_t, std::size_t) -> void override {}
			inline auto do_is_equal(const std::pmr::memory_resource &other) const noexcept -> bool override {
				const DoubleStackMemoryResource *resource {dynamic_cast<const DoubleStackMemoryResource*> (&other)};
				if (resource == nullptr)
					return false;
				return m_allocator == resource->m_allocator;
			}

			DoubleStackAllocator *m_allocator;
	};

} // namespace sl::memory
