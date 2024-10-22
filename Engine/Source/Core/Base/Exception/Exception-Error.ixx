module;
#include <ViseraEngine>

export module Visera.Core.Base.Exception:Error;

export namespace VE
{

	class Error : public std::exception
	{
	public:
		virtual
		const char*
		what()
		const override
		{
			return m_message.c_str();
		}

		virtual
		std::string
		where()
		const
		{
			std::stringstream ss;
			ss  << "\n[Error Location]"
				<< "\n- File: "		<< m_location.file_name()
				<< "\n- Line: "		<< m_location.line()
				<< "\n- Function: "	<< m_location.function_name();
			return ss.str();
		}

		explicit Error(
			const std::string& message,
			const std::source_location& location = std::source_location::current())
			:
			m_message	{ std::move(message) },
			m_location	{ std::move(location) }
		{

		}

	private:
		std::string m_message;
		std::source_location m_location;
	};

} // namespace VE