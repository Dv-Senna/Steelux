#pragma once

#include <functional>
#include <map>
#include <set>

#include "sl/core.hpp"
#include "sl/utils/hash.hpp"
#include "sl/utils/uuid.hpp"


namespace sl {
	using EventCategory = sl::utils::Hash64;
	using ListenerUUID = sl::utils::BasicUUID<std::uint64_t, static_cast<std::size_t> (sl::utils::hash<sl::utils::Hash64> (__FILE__, sizeof(__FILE__))) + __LINE__>;

	template <typename T>
	struct Event {
		T data;
	};


	struct EventFilter {
		std::set<EventCategory> categories;
		std::set<EventCategory> excludeCategories;
		std::set<UUID> sources;
		std::set<UUID> excludeSources;
	};


	class SL_CORE EventManager final {
		public:
			template <typename T>
			using ListenerCallback = std::function<auto (const std::set<EventCategory> &/*categories*/, UUID /*source*/, const Event<T> &/*event*/) -> void>;

			EventManager() = delete;

			template <typename T>
			static auto send(std::set<EventCategory> categories, UUID source, const Event<T> &event) noexcept -> void;

			template <typename T>
			[[nodiscard]]
			static auto addListener(const EventFilter &filter, const ListenerCallback<T> &callback) noexcept -> ListenerUUID;
			template <typename T>
			static auto removeListener(ListenerUUID listener) noexcept -> void;

			template <typename T>
			static auto setListenerCallback(ListenerUUID listener, const ListenerCallback<T> &callback) noexcept -> void;
			template <typename T>
			static auto setListenerFilter(ListenerUUID listener, const EventFilter &filter) noexcept -> void;

		private:
			struct Listener {
				void *callback;
				EventFilter filter;
			};

			static std::map<ListenerUUID, Listener> s_listeners;
	};

	namespace literals {
		constexpr auto operator ""_ecat(const char *str, std::size_t N) noexcept -> EventCategory {return sl::utils::hash<sl::utils::Hash64> (str, N);}
	} // namespace literals

} // namespace sl

#include "sl/eventManager.inl"
