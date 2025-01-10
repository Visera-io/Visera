module;
#include <Visera.h>
#include <spdlog/spdlog.h>

export module Visera.Core.Log;
import :SystemLogger;
import :AppLogger;

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
	{SystemLogger::GetInstance().Info(Formatter, std::forward<Args>(Arguments)...);}

	static inline void
	Info(const String& Message)
	{SystemLogger::GetInstance().Info(Message);}

	template<typename... Args>
	static inline void
	Warn(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
	{SystemLogger::GetInstance().Warn(Formatter, std::forward<Args>(Arguments)...);}

	static inline void
	Warn(const String& Message)
	{SystemLogger::GetInstance().Warn(Message);}

	template<typename... Args>
	static inline void
	Error(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
	{SystemLogger::GetInstance().Error(Formatter, std::forward<Args>(Arguments)...);}

	static inline void
	Error(const String& Message)
	{SystemLogger::GetInstance().Error(Message);}

	static inline void
	Fatal(const String& Message, const std::source_location& location = std::source_location::current())
	{SystemLogger::GetInstance().Fatal(Message, location);}

	template<typename... Args>
	static inline void
	Debug(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
	{SystemLogger::GetInstance().Debug(Formatter, std::forward<Args>(Arguments)...);}

	static inline void
	Debug(const String& Message)
	{SystemLogger::GetInstance().Debug(Message);}
};
} // namespace VE

export namespace VISERA_APP_NAMESPACE
{
	using namespace VE;
	class Log
	{
	public:
		template<typename... Args>
		static inline void
		Info(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{AppLogger::GetInstance().Info(Formatter, std::forward<Args>(Arguments)...);}

		static inline void
		Info(const String& Message)
		{AppLogger::GetInstance().Info(Message);}

		template<typename... Args>
		static inline void
		Warn(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{AppLogger::GetInstance().Warn(Formatter, std::forward<Args>(Arguments)...);}

		static inline void
		Warn(const String& Message)
		{AppLogger::GetInstance().Warn(Message);}

		template<typename... Args>
		static inline void
		Error(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{AppLogger::GetInstance().Error(Formatter, std::forward<Args>(Arguments)...);}

		static inline void
		Error(const String& Message)
		{AppLogger::GetInstance().Error(Message);}

		static inline void
		Fatal(const String& Message, const std::source_location& location = std::source_location::current())
		{AppLogger::GetInstance().Fatal(Message, location);}

		template<typename... Args>
		static inline void
		Debug(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{AppLogger::GetInstance().Debug(Formatter, std::forward<Args>(Arguments)...);}

		static inline void
		Debug(const String& Message)
		{AppLogger::GetInstance().Debug(Message);}
	};
} // namespace VISERA_APP_NAMESPACE