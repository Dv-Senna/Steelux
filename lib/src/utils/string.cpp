#include "sl/utils/string.hpp"


namespace sl::utils {
	String::String(const ConcatStringView &concatView) noexcept :
		m_allocator {concatView.m_strings.front()->copyAllocator()},
		m_size {}
	{
		m_heap.capacity = 0;

		std::ptrdiff_t size {};
		for (const String *string : concatView.m_strings)
			size += string->getSize();
		this->reserve(size);
		m_size = size;
		if (this->m_isSSO() || m_size <= MAX_SSO_SIZE)
			m_size *= -1;

		std::ptrdiff_t currentPosition {0};
		for (const String *string : concatView.m_strings) {
			std::memcpy(this->begin().getPtr() + currentPosition, string->begin().getPtr(), string->getSize());
			currentPosition += string->getSize();
		}
		this->begin().getPtr()[this->getSize()] = '\0';
	}


	String::String(const String &str) noexcept :
		m_allocator {str.m_allocator->copy()},
		m_size {str.m_size}
	{
		if (this->m_isSSO()) {
			std::memcpy(m_sso.buffer, str.m_sso.buffer, str.getSize() + 1);
			return;
		}

		m_heap.capacity = str.getSize() + 1;
		m_heap.start = reinterpret_cast<char*> (m_allocator->allocate(m_heap.capacity));
		std::memcpy(m_heap.start, str.m_heap.start, m_heap.capacity);
	}


	const String &String::operator=(const String &str) noexcept {
		this->~String();

		m_allocator = str.m_allocator->copy();
		m_size = str.m_size;

		if (this->m_isSSO()) {
			std::memcpy(m_sso.buffer, str.m_sso.buffer, str.getSize() + 1);
			return *this;
		}

		m_heap.capacity = str.getSize() + 1;
		m_heap.start = reinterpret_cast<char*> (m_allocator->allocate(m_heap.capacity));
		return *this;
	}


	String::String(String &&str) noexcept :
		m_allocator {std::move(str.m_allocator)},
		m_size {str.m_size}
	{
		if (this->m_isSSO()) {
			std::memcpy(m_sso.buffer, str.m_sso.buffer, str.getSize() + 1);
			std::memset(str.m_sso.buffer, 0, sizeof(m_heap));
			str.m_size = 0;
			return;
		}

		m_heap.capacity = str.m_heap.capacity;
		m_heap.start = str.m_heap.start;
		str.m_heap.capacity = 0;
		str.m_heap.start = nullptr;
		str.m_size = 0;
	}


	const String &String::operator=(String &&str) noexcept {
		this->~String();

		m_allocator = std::move(str.m_allocator);
		m_size = str.m_size;

		if (this->m_isSSO()) {
			std::memcpy(m_sso.buffer, str.m_sso.buffer, MAX_SSO_SIZE + 1);
			std::memset(str.m_sso.buffer, 0, sizeof(m_heap));
			str.m_size = 0;
			return *this;
		}

		m_heap.capacity = str.m_heap.capacity;
		m_heap.start = str.m_heap.start;
		str.m_heap.capacity = 0;
		str.m_heap.start = nullptr;
		str.m_size = 0;
		return *this;
	}


	String &String::operator+=(const String &string) noexcept {
		if (string.isEmpty())
			return *this;

		const std::ptrdiff_t totalSize {this->getSize() + string.getSize()};
		if (totalSize + 1 > this->getCapacity())
			this->reserve(totalSize);
		std::memcpy(this->begin().getPtr() + this->getSize(), string.begin().getPtr(), string.getSize() + 1);
		m_size = totalSize;
		return *this;
	}





	ConcatStringView::ConcatStringView() noexcept : m_strings {} {
		m_strings.reserve(4);
	}

	ConcatStringView::~ConcatStringView() {}


	ConcatStringView::ConcatStringView(const ConcatStringView &view) noexcept : m_strings {view.m_strings} {}


	const ConcatStringView &ConcatStringView::operator=(const ConcatStringView &view) noexcept {
		m_strings = view.m_strings;
		return *this;
	}


	ConcatStringView::ConcatStringView(ConcatStringView &&view) noexcept : m_strings {std::move(view.m_strings)} {}


	const ConcatStringView &ConcatStringView::operator=(ConcatStringView &&view) noexcept {
		m_strings = std::move(view.m_strings);
		return *this;
	}


	void ConcatStringView::pushFront(const String &string) noexcept {
		m_strings.insert(m_strings.begin(), &string);
	}


	void ConcatStringView::pushFront(const ConcatStringView &view) noexcept {
		m_strings.insert(m_strings.begin(), view.m_strings.begin(), view.m_strings.end());
	}


	void ConcatStringView::pushBack(const String &string) noexcept {
		m_strings.push_back(&string);
	}


	void ConcatStringView::pushBack(const ConcatStringView &view) noexcept {
		m_strings.insert(m_strings.end(), view.m_strings.begin(), view.m_strings.end());
	}

} // namespace sl::utils
