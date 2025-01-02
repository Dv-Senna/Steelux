#pragma once

#include <memory>
#include <print>

#include "sl/application.hpp"
#include "sl/utils/logger.hpp"

auto createApplication() noexcept -> std::unique_ptr<sl::Application>;



auto main(int, char**) -> int {
	auto application {createApplication()};
	sl::Result applicationCreationResult {application->create()};
	if (applicationCreationResult != sl::Result::eSuccess) {
		sl::mainLogger.error("Can't create application : {}", sl::utils::toString(applicationCreationResult));
		return EXIT_FAILURE;
	}
	application->destroy();
	return EXIT_SUCCESS;
}
