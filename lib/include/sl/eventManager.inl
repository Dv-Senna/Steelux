#pragma once

#include <ranges>

#include "sl/eventManager.hpp"
#include "sl/utils/logger.hpp"


namespace sl {
	template <typename T>
	auto EventManager::send(std::set<EventCategory> categories, UUID source, const Event<T> &event) noexcept -> void {
		for (const auto &listener : s_listeners) {

			bool hasCategories {false};
			if (listener.second.filter.categories.empty())
				hasCategories = true;
			else if (categories.empty())
				hasCategories = false;
			else {
				hasCategories = std::ranges::fold_left(categories | std::views::transform([&listener](EventCategory category) -> bool {
					return listener.second.filter.categories.find(category) != listener.second.filter.categories.end();
				}), true, std::logical_and<bool> ());
			}

			if (!hasCategories)
				continue;

			bool hasExcludeCategories {std::ranges::fold_left(categories | std::views::transform([&listener](EventCategory category) -> bool {
				return listener.second.filter.excludeCategories.find(category) != listener.second.filter.excludeCategories.end();
			}), false, std::logical_or<bool> ())};

			if (hasExcludeCategories)
				continue;

			if (!listener.second.filter.sources.empty() && listener.second.filter.sources.find(source) == listener.second.filter.sources.end())
				continue;
			if (listener.second.filter.excludeSources.find(source) != listener.second.filter.excludeSources.end())
				continue;

			(*reinterpret_cast<EventManager::ListenerCallback<T>*> (listener.second.callback)) (categories, source, event);
		}
	}


	template <typename T>
	auto EventManager::addListener(const EventFilter &filter, const ListenerCallback<T> &callback) noexcept -> ListenerUUID {
		ListenerUUID uuid {ListenerUUID::generate()};
		s_listeners[uuid] = {new ListenerCallback<T> (callback), filter};
		return uuid;
	}


	template <typename T>
	auto EventManager::removeListener(ListenerUUID listener) noexcept -> void {
		auto it {s_listeners.find(listener)};
		if (it == s_listeners.end())
			return;
		delete reinterpret_cast<ListenerCallback<T>*> (it->second.callback);
		s_listeners.erase(it);
	}


	template <typename T>
	auto EventManager::setListenerCallback(ListenerUUID listener, const ListenerCallback<T> &callback) noexcept -> void {
		auto it {s_listeners.find(listener)};
		if (it == s_listeners.end())
			return;
		*reinterpret_cast<ListenerCallback<T>*> (it->second.callback) = callback;
	}


	template <typename T>
	auto EventManager::setListenerFilter(ListenerUUID listener, const EventFilter &filter) noexcept -> void {
		auto it {s_listeners.find(listener)};
		if (it == s_listeners.end())
			return;
		it->second.filter = filter;
	}

} // namespace sl
