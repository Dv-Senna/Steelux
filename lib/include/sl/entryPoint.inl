#pragma once

#include <memory>
#include <print>

#include "sl/application.hpp"
#include "sl/utils/errorStack.hpp"
#include "sl/utils/logger.hpp"

auto createApplication() noexcept -> std::unique_ptr<sl::Application>;


auto logError() noexcept -> void {
	if (sl::utils::ErrorStack::isEmpty())
		return;

	sl::mainLogger.error("The following error happened in the program (from last to first) :");
	for (const sl::utils::ErrorInfos &error : sl::utils::ErrorStack::range()) {
		sl::mainLogger.error("\t{} ({}:{}) : {} > {}",
			error.location.function_name(),
			error.location.file_name(),
			error.location.line(),
			error.result,
			error.text
		);
	}
}


auto main(int, char**) -> int {
	auto application {createApplication()};
	sl::Result applicationCreationResult {application->create()};
	if (applicationCreationResult != sl::Result::eSuccess) {
		(void)sl::utils::ErrorStack::push(applicationCreationResult, "Can't create application");
		logError();
		return EXIT_FAILURE;
	}
	application->destroy();
	return EXIT_SUCCESS;
}
