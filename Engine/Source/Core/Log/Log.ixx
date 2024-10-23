module;
#include <ViseraEngine>
#include <spdlog/spdlog.h>

export module Visera.Core.Log;
import :Logger;

export namespace VE
{
	/*
	[Levels] 1.Debug 2.Info 3.Warn 4.Error 5.Fatal(throw VE::RuntimeError)
	*/
	class Log
	{
	public:
		template<typename... Args>
		static inline void
		Info(spdlog::format_string_t<Args...> fmt, Args &&...args)
		{Logger::GetInstance().Info(fmt, std::forward<Args>(args)...);}

		static inline void
		Info(const std::string& message)
		{Logger::GetInstance().Info(message);}

		template<typename... Args>
		static inline void
		Warn(spdlog::format_string_t<Args...> fmt, Args &&...args)
		{Logger::GetInstance().Warn(fmt, std::forward<Args>(args)...);}

		static inline void
		Warn(const std::string& message)
		{Logger::GetInstance().Warn(message);}

		template<typename... Args>
		static inline void
		Error(spdlog::format_string_t<Args...> fmt, Args &&...args)
		{Logger::GetInstance().Error(fmt, std::forward<Args>(args)...);}

		static inline void
		Error(const std::string& message)
		{Logger::GetInstance().Error(message);}

		template<typename... Args>
		static inline void
		Fatal(spdlog::format_string_t<Args...> fmt, Args &&...args)
		{Logger::GetInstance().Fatal(fmt, std::forward<Args>(args)...);}

		static inline void
		Fatal(const std::string& message, const std::source_location& location = std::source_location::current())
		{Logger::GetInstance().Fatal(message, location);}

		template<typename... Args>
		static inline void
		Debug(spdlog::format_string_t<Args...> fmt, Args &&...args)
		{Logger::GetInstance().Debug(fmt, std::forward<Args>(args)...);}

		static inline void
		Debug(const std::string& message)
		{Logger::GetInstance().Debug(message);}
	};
} // namespace VE
