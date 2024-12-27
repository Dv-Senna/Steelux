#pragma once

#include "sl/memory/allocator.hpp"


namespace sl::memory {
	template <typename Alloc>
	AllocatorViewFactory<Alloc>::AllocatorViewFactory() noexcept :
		m_allocator {nullptr}
	{

	}


	template <typename Alloc>
	AllocatorViewFactory<Alloc>::~AllocatorViewFactory() {

	}


	template <typename Alloc>
	AllocatorViewFactory<Alloc>::AllocatorViewFactory(Alloc &allocator) noexcept :
		m_allocator {&allocator}
	{

	}


	template <typename Alloc>
	std::byte *AllocatorViewFactory<Alloc>::allocate(std::size_t n) noexcept {
		if (m_allocator != nullptr)
			return reinterpret_cast<std::byte*> (m_allocator->allocate(n));
		Alloc allocator {};
		return reinterpret_cast<std::byte*> (allocator.allocate(n));
	}


	template <typename Alloc>
	void AllocatorViewFactory<Alloc>::deallocate(std::byte *ptr, std::size_t n) noexcept {
		if (m_allocator != nullptr)
			return m_allocator->deallocate(reinterpret_cast<Alloc::value_type*> (ptr), n);
		Alloc allocator {};
		allocator.deallocate(reinterpret_cast<Alloc::value_type*> (ptr), n);
	}


	template <typename Alloc>
	std::unique_ptr<AllocatorView> AllocatorViewFactory<Alloc>::copy() noexcept {
		if (m_allocator == nullptr)
			return std::make_unique<AllocatorViewFactory<Alloc>> ();
		return std::make_unique<AllocatorViewFactory<Alloc>> (*m_allocator);
	}

} // namespace sl::memory
