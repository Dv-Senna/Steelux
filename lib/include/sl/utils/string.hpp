#pragma once

#include <cstddef>
#include <memory>
#include <ranges>
#include <vector>

#include "sl/core.hpp"
#include "sl/memory/allocator.hpp"
#include "sl/utils/iterator.hpp"


namespace sl::utils {
	class ConcatStringView;

	/**
	 * @brief A class that handles strings in Steelux
	 */
	class SL_CORE String final {
		public:
			using iterator = sl::utils::ContinousIterator<String, char>;
			using const_iterator = sl::utils::ContinousIterator<const String, const char>;
			using reverse_iterator = sl::utils::ReverseContinousIterator<String, char>;
			using const_reverse_iterator = sl::utils::ReverseContinousIterator<const String, const char>;

			template <typename Alloc = sl::memory::DefaultAllocator<char>>
			String(Alloc *allocator = nullptr) noexcept;
			template <typename Alloc = sl::memory::DefaultAllocator<char>>
			String(const char *str, Alloc *allocator = nullptr) noexcept;
			template <typename Alloc = sl::memory::DefaultAllocator<char>>
			String(const char *str, std::ptrdiff_t length, Alloc *allocator = nullptr) noexcept;

			String(const ConcatStringView &concatView) noexcept;

			~String();

			String(const String &str) noexcept;
			const String &operator=(const String &str) noexcept;

			String(String &&str) noexcept;
			const String &operator=(String &&str) noexcept;

			/**
			 * @brief Reserve a certain amount of space for the string
			 * @param newSize The amount of space to reserve, not including the null-terminating character
			 * @return The amount of space the string has available. May be bigger than `newSize`. Not including the
			 *         null-terminating character
			 */
			std::ptrdiff_t reserve(std::ptrdiff_t newSize) noexcept;

			inline char *getData() noexcept;
			inline const char *getData() const noexcept;
			inline std::ptrdiff_t getSize() const noexcept;
			inline std::ptrdiff_t getCapacity() const noexcept;
			inline std::unique_ptr<sl::memory::AllocatorView> copyAllocator() const noexcept {return m_allocator->copy();}

			inline bool isEmpty() const noexcept {return m_size == 0;}

			inline char &operator[](std::ptrdiff_t index) noexcept;
			inline const char &operator[](std::ptrdiff_t index) const noexcept;

			iterator at(std::ptrdiff_t index) noexcept;
			const_iterator at(std::ptrdiff_t index) const noexcept;

			iterator insert(std::ptrdiff_t index, char value) noexcept;
			inline iterator insert(iterator position, char value) noexcept {return this->insert(position - this->begin(), value);}
			inline iterator insert(reverse_iterator position, char value) noexcept {return this->insert(position - this->rbegin(), value);}
			inline iterator pushFront(char value) noexcept {return this->insert(0, value);}
			inline iterator pushBack(char value) noexcept {return this->insert(this->getSize(), value);}

			void erase(std::ptrdiff_t index) noexcept;
			inline void erase(iterator position) noexcept {this->erase(position - this->begin());}
			inline void erase(reverse_iterator position) noexcept {this->erase(position - this->rbegin());}
			void erase(std::ptrdiff_t start, std::ptrdiff_t end) noexcept;
			inline void erase(iterator start, iterator end) noexcept {this->erase(start - this->begin(), end - this->begin());}
			inline void erase(reverse_iterator start, reverse_iterator end) noexcept {this->erase(start - this->rbegin(), end - this->rbegin());}
			inline void popFront() noexcept {this->erase(0);}
			inline void popBack() noexcept {this->erase(m_size - 1);}

			String &operator+=(const String &string) noexcept;

			inline iterator begin() noexcept;
			inline iterator end() noexcept;
			inline const_iterator cbegin() const noexcept;
			inline const_iterator cend() const noexcept;
			inline const_iterator begin() const noexcept {return this->cbegin();}
			inline const_iterator end() const noexcept {return this->cend();}
			inline reverse_iterator rbegin() noexcept;
			inline reverse_iterator rend() noexcept;
			inline const_reverse_iterator crbegin() const noexcept;
			inline const_reverse_iterator crend() const noexcept;
			inline const_reverse_iterator rbegin() const noexcept {return this->crbegin();}
			inline const_reverse_iterator rend() const noexcept {return this->crend();}


		private:
			inline bool m_isSSO() const noexcept;

			std::unique_ptr<sl::memory::AllocatorView> m_allocator;
			// not counting null-terminating character
			std::ptrdiff_t m_size;

			union {
				struct {
					char *start;
					std::ptrdiff_t capacity;
				} m_heap;

				struct {
					char buffer[sizeof(m_heap) / sizeof(char)];
				} m_sso;
			};

			static constexpr std::ptrdiff_t MAX_SSO_SIZE {sizeof(m_heap) / sizeof(char) - 1};
	};


	static_assert(std::ranges::random_access_range<String>, "String type must fullfill std::ranges::random_access_range concept");



	class SL_CORE ConcatStringView final {
		friend class String;

		public:
			ConcatStringView() noexcept;
			~ConcatStringView();

			ConcatStringView(const ConcatStringView &view) noexcept;
			const ConcatStringView &operator=(const ConcatStringView &view) noexcept;

			ConcatStringView(ConcatStringView &&view) noexcept;
			const ConcatStringView &operator=(ConcatStringView &&view) noexcept;

			void pushFront(const String &string) noexcept;
			void pushFront(const ConcatStringView &view) noexcept;
			void pushBack(const String &string) noexcept;
			void pushBack(const ConcatStringView &view) noexcept;


		private:
			std::vector<const String*> m_strings;
	};

	inline ConcatStringView operator+(const String &lhs, const String &rhs) noexcept {
		ConcatStringView view {};
		view.pushBack(lhs);
		view.pushBack(rhs);
		return view;
	}

	inline ConcatStringView operator+(ConcatStringView lhs, const String &rhs) noexcept {
		lhs.pushBack(rhs);
		return lhs;
	}

	inline ConcatStringView operator+(const String &lhs, ConcatStringView rhs) noexcept {
		rhs.pushFront(lhs);
		return rhs;
	}

	inline ConcatStringView operator+(ConcatStringView lhs, const ConcatStringView &rhs) noexcept {
		lhs.pushBack(rhs);
		return lhs;
	}


	namespace literals {
		constexpr sl::utils::String operator ""_s(const char *str, std::size_t length);
	} // namespace literals


	inline std::ostream &operator<<(std::ostream &stream, const sl::utils::String &string) noexcept {
		if (string.getData() != nullptr)
			stream << string.getData();
		return stream;
	}

	inline std::ostream &operator<<(std::ostream &stream, const sl::utils::ConcatStringView &view) noexcept {
		stream << sl::utils::String(view);
		return stream;
	}

} // namespace sl::utils


template <>
class std::formatter<sl::utils::String> : public std::formatter<std::string> {
	public:
		inline auto format(const sl::utils::String &string, std::format_context &ctx) const noexcept {
			if (string.getData() == nullptr)
				return std::formatter<std::string>::format("", ctx);
			return std::formatter<std::string>::format(string.getData(), ctx);
		}
};

template <>
class std::formatter<sl::utils::ConcatStringView> : public std::formatter<sl::utils::String> {
	public:
		inline auto format(const sl::utils::ConcatStringView &view, std::format_context &ctx) const noexcept {
			return std::formatter<sl::utils::String>::format(sl::utils::String(view), ctx);
		}
};


#include "sl/utils/string.inl"
