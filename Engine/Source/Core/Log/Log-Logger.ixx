module;
#include <ViseraEngine>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

export module Visera.Core.Log:Logger;

import Visera.Core.Base;
import Visera.Internal.Pattern;

export namespace VE
{
	class Logger:
		public Singleton<Logger>
	{
		friend class Singleton<Logger>;
	public:
		inline void
		Info(const std::string& message)
		{ m_handle->info(message); }

		template<typename... Args>
		inline void
		Info(spdlog::format_string_t<Args...> fmt, Args &&...args)
		{ m_handle->info(fmt, std::forward<Args>(args)...); }

		inline void
		Warn(const std::string& message)
		{ m_handle->warn(message); }

		template<typename... Args>
		inline void
		Warn(spdlog::format_string_t<Args...> fmt, Args &&...args)
		{ m_handle->warn(fmt, std::forward<Args>(args)...); }

		inline void
		Error(const std::string& message)
		{ m_handle->error(message); }

		template<typename... Args>
		inline void
		Error(spdlog::format_string_t<Args...> fmt, Args &&...args)
		{ m_handle->error(fmt, std::forward<Args>(args)...); }

		inline void
		Fatal(const std::string& message, const std::source_location& location)
		{ 
			m_handle->critical(message);
			throw RuntimeError("(Automatically thrown by Log::Fatal)", location);
		}

		template<typename... Args>
		inline void
		Fatal(spdlog::format_string_t<Args...> fmt, Args &&...args)
		{ m_handle->critical(fmt, std::forward<Args>(args)...); }

		inline void
		Debug(const std::string& message)
		{ m_handle->debug(message); }

		template<typename... Args>
		inline void
		Debug(spdlog::format_string_t<Args...> fmt, Args &&...args)
		{ m_handle->debug(fmt, std::forward<Args>(args)...); }

		Logger() noexcept
		{
			auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
 			m_handle = std::make_unique<spdlog::logger>("Visera Log", console_sink);
 #ifndef NDEBUG
 			m_handle->set_level(spdlog::level::debug);
 #else
 			m_handle->set_level(spdlog::level::warn);
 #endif
 			//m_handle->set_pattern("[%^%l%$] [%Y-%m-%d %H:%M:%S] %v");
 			m_handle->set_pattern("%^[Visera - %l - %H:%M:%S - Thread:%t]%$\n%v");
 			m_handle->debug("System logger is begin initialized.");
		}
		virtual ~Logger() noexcept
		{
			m_handle->debug("Releasing the system logger...");
			m_handle->flush();

			//Do not call drop_all() in your class!
			spdlog::drop_all();
			m_handle.reset();
		}

	protected:
		UniquePtr<spdlog::logger> m_handle;
	};
}
