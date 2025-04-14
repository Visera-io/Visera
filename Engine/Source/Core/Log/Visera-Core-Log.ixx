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
		template<typename... Args> static void
		Info(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{FAppLogger::GetInstance().Info(Formatter, std::forward<Args>(Arguments)...);}

		template<typename... Args> static void
		Warn(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{FAppLogger::GetInstance().Warn(Formatter, std::forward<Args>(Arguments)...);}

		template<typename... Args> static void
		Error(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{FAppLogger::GetInstance().Error(Formatter, std::forward<Args>(Arguments)...);}

		template<typename... Args> static void
		Fatal(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{FAppLogger::GetInstance().Fatal(Formatter, std::forward<Args>(Arguments)...);}

		template<typename... Args> static void
		Debug(spdlog::format_string_t<Args...> Formatter, Args &&...Arguments)
		{FAppLogger::GetInstance().Debug(Formatter, std::forward<Args>(Arguments)...);}
	};

} // namespace VISERA_APP_NAMESPACE