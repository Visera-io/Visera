module;
#include <Visera.h>
#include <spdlog/spdlog.h>

export module Visera.Core.Log;
import :SystemLogger;
import :AppLogger;

export namespace VE
{

	class Log
	{
		VE_MODULE_MANAGER_CLASS(Log);
	public:
		template<typename... Args> static void
		Info(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{FSystemLogger::GetInstance().Info(Formatter, std::forward<Args>(Arguments)...);}

		template<typename... Args> static void
		Warn(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{FSystemLogger::GetInstance().Warn(Formatter, std::forward<Args>(Arguments)...);}

		template<typename... Args> static void
		Error(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{FSystemLogger::GetInstance().Error(Formatter, std::forward<Args>(Arguments)...);}

		template<typename... Args> static void
		Fatal(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{FSystemLogger::GetInstance().Fatal(Formatter, std::forward<Args>(Arguments)...);}

		template<typename... Args> static void
		Debug(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{FSystemLogger::GetInstance().Debug(Formatter, std::forward<Args>(Arguments)...);}
	};

} // namespace VE

export namespace VISERA_APP_NAMESPACE
{
	using namespace VE;
	class Log
	{
		VE_MODULE_MANAGER_CLASS(Log);
	public:
		template<typename... Args>
		static inline void
		Info(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{FAppLogger::GetInstance().Info(Formatter, std::forward<Args>(Arguments)...);}

		static inline void
		Info(const String& Message)
		{FAppLogger::GetInstance().Info(Message);}

		template<typename... Args>
		static inline void
		Warn(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{FAppLogger::GetInstance().Warn(Formatter, std::forward<Args>(Arguments)...);}

		static inline void
		Warn(const String& Message)
		{FAppLogger::GetInstance().Warn(Message);}

		template<typename... Args>
		static inline void
		Error(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{FAppLogger::GetInstance().Error(Formatter, std::forward<Args>(Arguments)...);}

		static inline void
		Error(const String& Message)
		{FAppLogger::GetInstance().Error(Message);}

		static inline void
		Fatal(const String& Message, const std::source_location& location = std::source_location::current())
		{FAppLogger::GetInstance().Fatal(Message, location);}

		template<typename... Args>
		static inline void
		Debug(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{FAppLogger::GetInstance().Debug(Formatter, std::forward<Args>(Arguments)...);}

		static inline void
		Debug(const String& Message)
		{FAppLogger::GetInstance().Debug(Message);}
	};
} // namespace VISERA_APP_NAMESPACE