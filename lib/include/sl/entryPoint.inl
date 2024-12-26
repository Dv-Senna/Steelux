#pragma once

#include <memory>
#include <print>

#include "sl/application.hpp"

std::unique_ptr<sl::Application> createApplication();



int main(int, char**) {
	auto application {createApplication()};
	if (application->create() != sl::Result::eSuccess) {
		std::println(stderr, "Can't create application");
		return EXIT_FAILURE;
	}
	application->destroy();
	return EXIT_SUCCESS;
}
