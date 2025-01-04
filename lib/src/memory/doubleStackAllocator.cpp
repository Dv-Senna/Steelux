#include "sl/memory/doubleStackAllocator.hpp"


namespace sl::memory {
	DoubleStackAllocator::DoubleStackAllocator(sl::utils::Bytes size) noexcept :
		m_stackSize {size},
		m_stackBase {reinterpret_cast<pointer> (std::malloc(m_stackSize))},
		m_topStackTop {m_stackBase + size},
		m_bottomStackTop {m_stackBase},
		m_isTopStackActive {false}
	{

	}


	DoubleStackAllocator::~DoubleStackAllocator() {
		if (m_stackBase != nullptr)
			std::free(m_stackBase);
		m_stackSize = 0_B;
		m_bottomStackTop = nullptr;
		m_topStackTop = nullptr;
		m_stackBase = nullptr;
		m_isTopStackActive = false;
	}


	DoubleStackAllocator::DoubleStackAllocator(DoubleStackAllocator &&allocator) noexcept :
		m_stackSize {allocator.m_stackSize},
		m_stackBase {allocator.m_stackBase},
		m_topStackTop {allocator.m_topStackTop},
		m_bottomStackTop {allocator.m_bottomStackTop},
		m_isTopStackActive {allocator.m_isTopStackActive}
	{
		allocator.m_stackSize = 0_B;
		allocator.m_stackBase = nullptr;
		allocator.m_topStackTop = nullptr;
		allocator.m_bottomStackTop = nullptr;
		allocator.m_isTopStackActive = false;
	}


	auto DoubleStackAllocator::operator=(DoubleStackAllocator &&allocator) noexcept -> DoubleStackAllocator& {
		this->~DoubleStackAllocator();

		m_stackSize = allocator.m_stackSize;
		m_stackBase = allocator.m_stackBase;
		m_topStackTop = allocator.m_topStackTop;
		m_bottomStackTop = allocator.m_bottomStackTop;
		m_isTopStackActive = allocator.m_isTopStackActive;

		allocator.m_stackSize = 0_B;
		allocator.m_stackBase = nullptr;
		allocator.m_topStackTop = nullptr;
		allocator.m_bottomStackTop = nullptr;
		allocator.m_isTopStackActive = false;

		return *this;
	}


	[[nodiscard]]
	auto DoubleStackAllocator::allocate(size_type size, size_type alignment) noexcept -> pointer {
		if (m_isTopStackActive) {
			pointer tmpStackTop {m_topStackTop};
			tmpStackTop -= reinterpret_cast<size_type> (tmpStackTop) % alignment;

			if ((tmpStackTop - size) - m_bottomStackTop < 0)
				return nullptr;

			m_topStackTop = tmpStackTop - size;
			return tmpStackTop;
		}

		pointer tmpStackTop {m_bottomStackTop};
		if (reinterpret_cast<size_type> (tmpStackTop) % alignment != 0)
			tmpStackTop += alignment - (reinterpret_cast<size_type> (tmpStackTop) % alignment);

		if (m_topStackTop - (tmpStackTop + size) < 0)
			return nullptr;

		m_bottomStackTop = tmpStackTop + size;
		return tmpStackTop;
	}


	[[nodiscard]]
	auto DoubleStackAllocator::mark() const noexcept -> Marker {
		return DoubleStackAllocatorMarker(m_isTopStackActive ? m_topStackTop : m_bottomStackTop, m_isTopStackActive);
	}


	auto DoubleStackAllocator::deallocate(Marker marker) noexcept -> void {
		if (marker.m_isFromTopStack)
			m_topStackTop = marker.m_marker;
		else
			m_bottomStackTop = marker.m_marker;
	}


	auto DoubleStackAllocator::swapActiveStack() noexcept -> void {
		m_isTopStackActive = !m_isTopStackActive;
	}

} // namespace sl::memory
