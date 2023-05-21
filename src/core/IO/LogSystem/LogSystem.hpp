#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace SPW
{
#define FS_LOGGER_INFO(...) SPW::LogSystem::GetIOLogger()->info(__VA_ARGS__);
#define FS_LOGGER_WRAN(...) SPW::LogSystem::GetIOLogger()->warn(__VA_ARGS__);
#define FS_LOGGER_CRITICAL(...) SPW::LogSystem::GetIOLogger()->critical(__VA_ARGS__);

#define CFG_LOGGER_INFO(...) SPW::LogSystem::GetConfigLogger()->info(__VA_ARGS__);
#define CFG_LOGGER_WRAN(...) SPW::LogSystem::GetConfigLogger()->warn(__VA_ARGS__);
#define CFG_LOGGER_CRITICAL(...) SPW::LogSystem::GetConfigLogger()->critical(__VA_ARGS__);

#define TEST_LOGGER_INFO(...) SPW::LogSystem::GetTestLogger()->info(__VA_ARGS__);
#define TEST_LOGGER_WRAN(...) SPW::LogSystem::GetTestLogger()->warn(__VA_ARGS__);
#define TEST_LOGGER_CRITICAL(...) SPW::LogSystem::GetTestLogger()->critical(__VA_ARGS__);

#define EDI_LOGGER_INFO(...) SPW::LogSystem::GetEditorLogger()->info(__VA_ARGS__);
#define EDI_LOGGER_WRAN(...) SPW::LogSystem::GetEditorLogger()->warn(__VA_ARGS__);
#define EDI_LOGGER_CRITICAL(...) SPW::LogSystem::GetEditorLogger()->critical(__VA_ARGS__);


#ifdef near
#undef near
#endif

#ifdef far
#undef far
#endif

	class LogSystem
	{
	public:
		static void LoggerInit();

		inline static std::shared_ptr<spdlog::logger>& GetIOLogger() { return ioLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetConfigLogger() { return cfgLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetTestLogger() { return testLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetEditorLogger() { return editorLogger; }
	private:
		static std::shared_ptr<spdlog::logger> ioLogger;
		static std::shared_ptr<spdlog::logger> cfgLogger;
		static std::shared_ptr<spdlog::logger> testLogger;
		static std::shared_ptr<spdlog::logger> editorLogger;
	};

}
