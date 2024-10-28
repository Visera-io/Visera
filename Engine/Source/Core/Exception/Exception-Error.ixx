module;
#include <ViseraEngine>

export module Visera.Core.Exception:Error;

export namespace VE
{

	class Error : public std::exception
	{
	public:
		virtual
		StringView
		What() const
		{ return Message; }

		virtual
		String
		Where() const
		{
			std::stringstream ss;
			ss  << "\n[Error Location]"
				<< "\n- File: "		<< Location.file_name()
				<< "\n- Line: "		<< Location.line()
				<< "\n- Function: "	<< Location.function_name();
			return ss.str();
		}

		explicit Error(
			const String& Message,
			const std::source_location& Location = std::source_location::current())
			:
			Message	{ std::move(Message) },
			Location	{ std::move(Location) }
		{

		}

	private:
		String Message;
		std::source_location Location;
	};

} // namespace VE