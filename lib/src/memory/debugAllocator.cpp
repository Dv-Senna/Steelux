#include "sl/memory/debugAllocator.hpp"

#include "sl/utils/logger.hpp"


namespace sl::memory {
	void __DebugAllocator_print(const std::string &typeName, std::size_t allocationCount, float averageMsBetweenAllocation) {
		sl::mainLogger.debug("DEBUG ALLOCATOR : {}\n\t- Count : {}\n\t- Time : {}[ms]", typeName, allocationCount, averageMsBetweenAllocation);
	}

} // namespace sl::memory
