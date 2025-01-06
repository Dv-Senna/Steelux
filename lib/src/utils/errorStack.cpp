#include "sl/utils/errorStack.hpp"


namespace sl::utils {
	auto ErrorStackIterator::operator++() noexcept -> ErrorStackIterator& {
		ErrorStack::s_stack.pop();
		if (ErrorStack::s_stack.empty()) {
			m_infos = nullptr;
			return *this;
		}

		m_infos = &ErrorStack::s_stack.top();
		return *this;
	}


	auto ErrorStack::clear() noexcept -> void {
		while (!s_stack.empty())
			s_stack.pop();
	}


	std::stack<ErrorInfos> ErrorStack::s_stack {};

} // namespace sl::utils
