#pragma once

#include "LogSystem.hpp"
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>
#define SPDLOG_HEADER_ONLY
// #include <iostream>

namespace SPW
{
	std::shared_ptr<spdlog::logger> LogSystem::ioLogger;
	std::shared_ptr<spdlog::logger> LogSystem::cfgLogger;
	std::shared_ptr<spdlog::logger> LogSystem::testLogger;

	// std::shared_ptr<spdlog::logger> ioLogger;

	void LogSystem::LoggerInit()
	{
        // Console logger with color
		spdlog::set_pattern("%^[%T] %n:%$ %v");
		ioLogger = spdlog::stdout_color_mt("IO");
		cfgLogger = spdlog::stdout_color_mt("Config");
		testLogger = spdlog::stdout_color_mt("Test");
//		ioLogger->set_level(spdlog::level::info);
	}
}
