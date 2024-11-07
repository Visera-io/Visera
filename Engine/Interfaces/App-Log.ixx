module;
#include <Visera>

#include <spdlog/spdlog.h>

export module Visera.App.Log;
import Visera.App.Base;

import Visera.Engine.Core.Log.Logger;

export namespace VISERA_APP_NAMESPACE
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
		{VE::AppLogger::GetInstance().Info(Formatter, std::forward<Args>(Arguments)...);}

		static inline void
		Info(const String& Message)
		{VE::AppLogger::GetInstance().Info(Message);}

		template<typename... Args>
		static inline void
		Warn(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{VE::AppLogger::GetInstance().Warn(Formatter, std::forward<Args>(Arguments)...);}

		static inline void
		Warn(const String& Message)
		{VE::AppLogger::GetInstance().Warn(Message);}

		template<typename... Args>
		static inline void
		Error(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{VE::AppLogger::GetInstance().Error(Formatter, std::forward<Args>(Arguments)...);}

		static inline void
		Error(const String& Message)
		{VE::AppLogger::GetInstance().Error(Message);}

		template<typename... Args>
		static inline void
		Fatal(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{VE::AppLogger::GetInstance().Fatal(Formatter, std::forward<Args>(Arguments)...);}

		static inline void
		Fatal(const String& Message, const std::source_location& location = std::source_location::current())
		{VE::AppLogger::GetInstance().Fatal(Message, location);}

		template<typename... Args>
		static inline void
		Debug(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{VE::AppLogger::GetInstance().Debug(Formatter, std::forward<Args>(Arguments)...);}

		static inline void
		Debug(const String& Message)
		{VE::AppLogger::GetInstance().Debug(Message);}

	};

} // namespace VISERA_APP_NAMESPACE