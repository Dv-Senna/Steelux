#include "sl/memory/stackAllocator.hpp"


namespace sl::memory {
	StackAllocator::StackAllocator(sl::utils::Bytes size) noexcept :
		m_stackSize {size},
		m_stackBase {reinterpret_cast<pointer> (std::malloc(m_stackSize))},
		m_stackTop {m_stackBase}
	{

	}


	StackAllocator::~StackAllocator() {
		if (m_stackBase != nullptr)
			std::free(m_stackBase);
		m_stackSize = 0_B;
		m_stackTop = nullptr;
		m_stackBase = nullptr;
	}


	StackAllocator::StackAllocator(StackAllocator &&allocator) noexcept :
		m_stackSize {allocator.m_stackSize},
		m_stackBase {allocator.m_stackBase},
		m_stackTop {allocator.m_stackTop}
	{
		allocator.m_stackSize = 0_B;
		allocator.m_stackBase = nullptr;
		allocator.m_stackTop = nullptr;
	}


	auto StackAllocator::operator=(StackAllocator &&allocator) noexcept -> StackAllocator& {
		this->~StackAllocator();

		m_stackSize = allocator.m_stackSize;
		m_stackBase = allocator.m_stackBase;
		m_stackTop = allocator.m_stackTop;

		allocator.m_stackSize = 0_B;
		allocator.m_stackBase = nullptr;
		allocator.m_stackTop = nullptr;

		return *this;
	}


	[[nodiscard]]
	auto StackAllocator::allocate(size_type size, size_type alignment) noexcept -> pointer {
		pointer tmpStackTop {m_stackTop};
		if (reinterpret_cast<size_type> (tmpStackTop) % alignment != 0)
			tmpStackTop += alignment - (reinterpret_cast<size_type> (tmpStackTop) % alignment);

		if ((m_stackBase + m_stackSize) - (tmpStackTop + size) < 0)
			return nullptr;

		m_stackTop = tmpStackTop + size;
		return tmpStackTop;
	}


	[[nodiscard]]
	auto StackAllocator::mark() const noexcept -> Marker {
		return StackAllocatorMarker(m_stackTop);
	}


	auto StackAllocator::deallocate(Marker marker) noexcept -> void {
		m_stackTop = marker.m_marker;
	}


	auto StackAllocator::clear() noexcept -> void {
		m_stackTop = m_stackBase;
	}

} // namespace sl::memory
