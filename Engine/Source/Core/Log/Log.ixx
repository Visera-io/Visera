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
		Info(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{Logger::GetInstance().Info(Formatter, std::forward<Args>(Arguments)...);}

		static inline void
		Info(const String& Message)
		{Logger::GetInstance().Info(Message);}

		template<typename... Args>
		static inline void
		Warn(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{Logger::GetInstance().Warn(Formatter, std::forward<Args>(Arguments)...);}

		static inline void
		Warn(const String& Message)
		{Logger::GetInstance().Warn(Message);}

		template<typename... Args>
		static inline void
		Error(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{Logger::GetInstance().Error(Formatter, std::forward<Args>(Arguments)...);}

		static inline void
		Error(const String& Message)
		{Logger::GetInstance().Error(Message);}

		template<typename... Args>
		static inline void
		Fatal(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{Logger::GetInstance().Fatal(Formatter, std::forward<Args>(Arguments)...);}

		static inline void
		Fatal(const String& Message, const std::source_location& location = std::source_location::current())
		{Logger::GetInstance().Fatal(Message, location);}

		template<typename... Args>
		static inline void
		Debug(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{Logger::GetInstance().Debug(Formatter, std::forward<Args>(Arguments)...);}

		static inline void
		Debug(const String& Message)
		{Logger::GetInstance().Debug(Message);}
	};
} // namespace VE
