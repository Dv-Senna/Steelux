#pragma once

#include <set>

#include "sl/utils/hash.hpp"


namespace sl {
	template <typename T>
	struct Event {
		T data;
	};

	class EventManager final {
		public:
			using Category = sl::utils::Hash64;

			EventManager() = delete;

			template <typename T>
			static auto send(std::set<Category> categories, void *source, const Event<T> &event);
	};

} // namespace sl
