module;
#include <Visera.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
export module Visera.Core.Log:AppLogger;

import Visera.Core.Signal;

export namespace VE
{

	class FAppLogger
	{
	public:
		static inline FAppLogger&
		GetInstance()
		{ static FAppLogger Singleton; return Singleton; }

		template<typename... Args>
		inline void
		Trace(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{ AppLogger->trace(Formatter, std::forward<Args>(Arguments)...); }

		template<typename... Args>
		inline void
		Debug(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{ AppLogger->debug(Formatter, std::forward<Args>(Arguments)...); }

		template<typename... Args>
		inline void
		Info(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{ AppLogger->info(Formatter, std::forward<Args>(Arguments)...); }

		template<typename... Args>
		inline void
		Warn(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{ AppLogger->warn(Formatter, std::forward<Args>(Arguments)...); }

		template<typename... Args>
		inline void
		Error(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{ AppLogger->error(Formatter, std::forward<Args>(Arguments)...); }

		template<typename... Args>
		inline void
		Fatal(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{
			AppLogger->critical(Formatter, std::forward<Args>(Arguments)...);
			throw SRuntimeError("A Fatal Error was triggered.");
		}

		FAppLogger() noexcept
		{
			auto ConsoleSink = CreateSharedPtr<spdlog::sinks::stdout_color_sink_mt>();

			auto RotatingSink = CreateSharedPtr<spdlog::sinks::rotating_file_sink_mt>(
				VISERA_APP_CACHE_DIR"/Logs/App.log",
				5 * OneMByte, // 5 MB per file, 
				3             // keep 3 backups
			);
			Array<spdlog::sink_ptr> Sinks { ConsoleSink, RotatingSink };

			AppLogger = CreateUniquePtr<spdlog::logger>("App Log", Sinks.begin(), Sinks.end());

	        AppLogger->set_level(spdlog::level::level_enum(VE_LOG_SYSTEM_VERBOSITY));

			AppLogger->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] [%L] [T:%t] %v%$");
		}
		virtual ~FAppLogger() noexcept
		{
			AppLogger->flush();

			//Do not call drop_all() in your class!
			spdlog::drop_all();
			AppLogger.reset();
		}

	protected:
		UniquePtr<spdlog::logger> AppLogger;
	};
} // namespace VE