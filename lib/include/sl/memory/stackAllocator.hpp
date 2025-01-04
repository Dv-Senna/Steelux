#pragma once

#include "sl/core.hpp"
#include "sl/memory/allocatorTraits.hpp"
#include "sl/utils/units.hpp"


namespace sl::memory {
	using namespace sl::utils::literals;

	class StackAllocator;


	class SL_CORE StackAllocatorMarker {
		friend class StackAllocator;

		public:
			using value_type = std::byte;
			using pointer = value_type*;

			constexpr StackAllocatorMarker() noexcept = default;
			constexpr ~StackAllocatorMarker() = default;
			constexpr StackAllocatorMarker(const StackAllocatorMarker &) noexcept = default;
			constexpr auto operator=(const StackAllocatorMarker &) noexcept -> StackAllocatorMarker& = default;
			constexpr StackAllocatorMarker(StackAllocatorMarker &&) noexcept = default;
			constexpr auto operator=(StackAllocatorMarker &&) noexcept -> StackAllocatorMarker& = default;


		protected:
			constexpr StackAllocatorMarker(pointer marker) noexcept : m_marker {marker} {}

			pointer m_marker;
	};


	class SL_CORE StackAllocator {
		public:
			using value_type = std::byte;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;
			using Marker = StackAllocatorMarker;

			StackAllocator(sl::utils::Bytes size = 16_MiB) noexcept;
			~StackAllocator();

			StackAllocator(const StackAllocator &) noexcept = delete;
			auto operator=(const StackAllocator &) noexcept -> StackAllocator& = delete;

			StackAllocator(StackAllocator &&allocator) noexcept;
			auto operator=(StackAllocator &&allocator) noexcept -> StackAllocator&;

			[[nodiscard]]
			auto allocate(size_type size, size_type alignment) noexcept -> pointer;

			[[nodiscard]]
			auto mark() const noexcept -> Marker;
			auto deallocate(Marker marker) noexcept -> void;

		private:
			sl::utils::Bytes m_stackSize;
			pointer m_stackBase;
			pointer m_stackTop;
	};


	template <typename T>
	class StackAllocatorView {
		public:
			using value_type = T;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			using reference = value_type&;
			using const_reference = const value_type&;
			using size_type = StackAllocator::size_type;
			using difference_type = StackAllocator::difference_type;
			using is_always_equal = std::false_type;

			constexpr StackAllocatorView(StackAllocator &allocator) : m_allocator {&allocator} {}
			constexpr ~StackAllocatorView() = default;

			constexpr StackAllocatorView(const StackAllocatorView<T> &) noexcept = default;
			constexpr auto operator=(const StackAllocatorView<T> &) noexcept -> StackAllocatorView<T>& = default;
			constexpr StackAllocatorView(StackAllocatorView<T> &&) noexcept = default;
			constexpr auto operator=(StackAllocatorView<T> &&) noexcept -> StackAllocatorView<T>& = default;

			[[nodiscard]]
			inline auto allocate(size_type n) const noexcept -> pointer {return reinterpret_cast<pointer> (m_allocator->allocate(sizeof(T) * n, alignof(T)));}
			inline auto deallocate(pointer, size_type) const noexcept -> void {}

			[[nodiscard]]
			inline auto mark() const noexcept -> StackAllocator::Marker {return m_allocator->mark();}
			inline auto deallocate(StackAllocator::Marker marker) noexcept -> void {m_allocator->deallocate(marker);}

			constexpr auto operator==(const StackAllocatorView<T> &view) const noexcept -> bool {return m_allocator == view.m_allocator;}

		private:
			StackAllocator *m_allocator;
	};


	static_assert(sl::memory::IsAllocator<StackAllocatorView<char>>);

} // namespace sl::memory
