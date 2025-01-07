#include "sl/eventManager.hpp"


namespace sl {
	std::map<ListenerUUID, EventManager::Listener> EventManager::s_listeners {};

} // namespace sl
