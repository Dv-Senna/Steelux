#pragma once

#include <memory>
#include <print>

#include "sl/application.hpp"

std::unique_ptr<sl::Application> createApplication();



int main(int, char**) {
	auto application {createApplication()};
	sl::Result applicationCreationResult {application->create()};
	if (applicationCreationResult != sl::Result::eSuccess) {
		std::println(stderr, "\033[31mCan't create application : {}\033[m", sl::utils::toString(applicationCreationResult));
		return EXIT_FAILURE;
	}
	application->destroy();
	return EXIT_SUCCESS;
}
