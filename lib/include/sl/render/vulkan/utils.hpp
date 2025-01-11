#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan.h>

#include "sl/utils/errorStack.hpp"


namespace sl::render::vulkan {
	template <typename Ret, typename ...Args, typename Func>
	inline auto getVulkanMultivalue(Func &&func, Args &&...args) -> std::optional<std::vector<Ret>> {
		if constexpr (std::same_as<std::invoke_result_t<Func, Args..., std::uint32_t*, Ret*>, VkResult>) {
			std::vector<Ret> values {};
			std::uint32_t count {};
			if (func(args..., &count, nullptr) != VK_SUCCESS)
				return sl::ErrorStack::push(std::nullopt, "Can't get count of vulkan multivalue");

			values.resize(count);
			if (func(args..., &count, values.data()) != VK_SUCCESS)
				return sl::ErrorStack::push(std::nullopt, "Can't get values of vulkan multivalue");
			return values;
		}
		else {
			std::vector<Ret> values {};
			std::uint32_t count {};
			(void)func(args..., &count, nullptr);
			values.resize(count);
			(void)func(args..., &count, values.data());
			return values;
		}
	}

} // namespace sl::render::vulkan
