module;
#include <Visera.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

export module Visera.Core.Log:AppLogger;
import Visera.Core.Signal;

VISERA_PUBLIC_MODULE
class AppLogger:
	public Singleton<AppLogger>
{
	friend class Singleton<AppLogger>;
public:
	inline void
	Info(const String& message)
	{ Spdlogger->info(message); }

	template<typename... Args>
	inline void
	Info(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
	{ Spdlogger->info(Formatter, std::forward<Args>(Arguments)...); }

	inline void
	Warn(const String& message)
	{ Spdlogger->warn(message); }

	template<typename... Args>
	inline void
	Warn(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
	{ Spdlogger->warn(Formatter, std::forward<Args>(Arguments)...); }

	inline void
	Error(const String& message)
	{ Spdlogger->error(message); }

	template<typename... Args>
	inline void
	Error(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
	{ Spdlogger->error(Formatter, std::forward<Args>(Arguments)...); }

	inline void
	Fatal(const String& Message, const std::source_location& Location)
	throw (AppStopSignal)
	{ 
		AppStopSignal Signal{ Message, Location };
		Spdlogger->critical("{}{}", Signal.What(), Signal.Where());
		throw Signal;
	}

	inline void
	Debug(const String& message)
	{ Spdlogger->debug(message); }

	template<typename... Args>
	inline void
	Debug(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
	{ Spdlogger->debug(Formatter, std::forward<Args>(Arguments)...); }

	AppLogger() noexcept
	{
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		Spdlogger = std::make_unique<spdlog::logger>("Visera Log", console_sink);
#ifndef NDEBUG
		Spdlogger->set_level(spdlog::level::debug);
#else
		Spdlogger->set_level(spdlog::level::warn);
#endif
		//m_handle->set_pattern("[%^%l%$] [%Y-%m-%d %H:%M:%S] %v
		Spdlogger->set_pattern("%^[" VISERA_APP_NAME " - %l - %H:%M:%S - Thread:%t]%$\n%v");
	}
	virtual ~AppLogger() noexcept
	{
		Spdlogger->flush();

		//Do not call drop_all() in your class!
		spdlog::drop_all();
		Spdlogger.reset();
	}

protected:
	UniquePtr<spdlog::logger> Spdlogger;
};
VISERA_MODULE_END