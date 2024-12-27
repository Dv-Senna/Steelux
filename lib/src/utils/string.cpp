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

		std::ptrdiff_t currentPosition {0};
		for (const String *string : concatView.m_strings) {
			std::memcpy(this->begin().getPtr() + currentPosition, string->begin().getPtr(), string->getSize());
			currentPosition += string->getSize();
		}
		this->begin().getPtr()[m_size] = '\0';
	}


	String::~String() {
		if (!this->m_isSSO())
			m_allocator->deallocate(reinterpret_cast<std::byte*> (m_heap.start), m_heap.capacity);
		m_size = 0;
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
			std::memcpy(m_sso.buffer, str.m_sso.buffer, str.getSize() + 1);
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


	std::ptrdiff_t String::reserve(std::ptrdiff_t newSize) noexcept {
		const std::ptrdiff_t capacity {this->getCapacity()};
		++newSize;
		if (newSize <= capacity)
			return capacity - 1;

		if (this->m_isSSO()) {
			char *buffer {reinterpret_cast<char*> (m_allocator->allocate(newSize))};
			std::memcpy(buffer, m_sso.buffer, sizeof(char) * newSize);
			m_heap.capacity = newSize;
			m_heap.start = buffer;
			m_size *= -1;
			return newSize - 1;
		}

		char *tmp {reinterpret_cast<char*> (m_allocator->allocate(newSize))};
		std::memcpy(tmp, m_heap.start, sizeof(char) * m_heap.capacity);
		m_allocator->deallocate(reinterpret_cast<std::byte*> (m_heap.start), m_heap.capacity);
		m_heap.capacity = newSize;
		m_heap.start = tmp;
		return newSize - 1;
	}


	String::iterator String::at(std::ptrdiff_t index) noexcept {
		if (index >= this->getSize())
			return this->end();
		if (index < -this->getSize())
			return this->end();
		if (index >= 0)
			return this->begin() + index;
		return this->end() + index;
	}


	String::const_iterator String::at(std::ptrdiff_t index) const noexcept {
		if (index >= this->getSize())
			return this->cend();
		if (index < -this->getSize())
			return this->cend();
		if (index >= 0)
			return this->cbegin() + index;
		return this->cend() + index;
	}


	String::iterator String::insert(std::ptrdiff_t index, char value) noexcept {
		constexpr float growFactor {1.5f};

		const std::ptrdiff_t capacity {this->getCapacity()};
		if (this->getSize() + 2 > capacity) {
			const std::ptrdiff_t newCapacity {static_cast<std::ptrdiff_t> (capacity * growFactor)};
			this->reserve(newCapacity);
		}

		auto it {this->begin()};
		it += index;
		std::memmove(it.getPtr() + 1, it.getPtr(), this->getSize() - index + 1);
		*it = value;
		if (this->m_isSSO())
			--m_size;
		else
			++m_size;
		return it;
	}


	void String::erase(std::ptrdiff_t index) noexcept {
		auto it {this->begin()};
		it += index;
		std::memmove(it.getPtr(), it.getPtr() + 1, this->getSize() - index);
		if (this->m_isSSO())
			++m_size;
		else
			--m_size;
	}


	void String::erase(std::ptrdiff_t start, std::ptrdiff_t end) noexcept {
		if (end <= start)
			return;
		auto it {this->begin()};
		it += start;
		std::memmove(it.getPtr(), it.getPtr() + end - 1, this->getSize() - end + 1);
		if (this->m_isSSO())
			m_size += end - start;
		else
			m_size -= end - start;
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
