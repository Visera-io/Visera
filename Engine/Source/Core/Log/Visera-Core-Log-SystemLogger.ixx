module;
#include <Visera.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
export module Visera.Core.Log:SystemLogger;

import Visera.Core.Signal;

export namespace VE
{

class FSystemLogger
{
public:
	static inline FSystemLogger&
	GetInstance()
	{ static FSystemLogger Singleton; return Singleton; }

	template<typename... Args>
	inline void
	Info(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
	{ SystemLogger->info(Formatter, std::forward<Args>(Arguments)...); }

	template<typename... Args>
	inline void
	Warn(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
	{ SystemLogger->warn(Formatter, std::forward<Args>(Arguments)...); }

	template<typename... Args>
	inline void
	Error(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
	{ SystemLogger->error(Formatter, std::forward<Args>(Arguments)...); }

	template<typename... Args>
	inline void
	Fatal(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
	{
		SystemLogger->critical(Formatter, std::forward<Args>(Arguments)...);
		throw SRuntimeError("A Fatal Error was triggered.");
	}

	template<typename... Args>
	inline void
	Debug(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
	{ SystemLogger->debug(Formatter, std::forward<Args>(Arguments)...); }

	FSystemLogger() noexcept
	{
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		SystemLogger = std::make_unique<spdlog::logger>("System Log", console_sink);

		SystemLogger->set_level(spdlog::level::level_enum(VE_LOG_SYSTEM_VERBOSITY));

		SystemLogger->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] [%L] [Thread:%t] %v%$");
	}
	virtual ~FSystemLogger() noexcept
	{
		SystemLogger->flush();

		//Do not call drop_all() in your class!
		spdlog::drop_all();
		SystemLogger.reset();
	}

protected:
	UniquePtr<spdlog::logger> SystemLogger;
};
} // namespace VE