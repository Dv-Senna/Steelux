#pragma once

#include <cstddef>
#include <expected>
#include <memory>


namespace sl::memory {
	class AllocatorView {
		public:
			AllocatorView() noexcept = default;
			virtual ~AllocatorView() = default;

			virtual std::byte *allocate(std::size_t n) noexcept = 0;
			virtual void deallocate(std::byte *ptr, std::size_t n) noexcept = 0;

			virtual std::unique_ptr<AllocatorView> copy() noexcept = 0;
	};


	template <typename Alloc>
	class AllocatorViewFactory final : public sl::memory::AllocatorView {
		public:
			// use the default constructor for stateless allocator
			AllocatorViewFactory() noexcept;
			~AllocatorViewFactory() override;

			// use this constructor for statefull allocator
			AllocatorViewFactory(Alloc &allocator) noexcept;

			std::byte *allocate(std::size_t n) noexcept override;
			void deallocate(std::byte *ptr, std::size_t n) noexcept override;

			std::unique_ptr<AllocatorView> copy() noexcept override;

		private:
			Alloc *m_allocator;
	};

} // namespace sl::memory

#include "sl/memory/allocator.inl"
