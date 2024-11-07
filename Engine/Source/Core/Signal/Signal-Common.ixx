module;
#include <Visera>

export module Visera.Engine.Core.Signal:Common;

export namespace VE
{

	class Signal : public std::exception
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
			ss  << "\n[Signal Location]"
				<< "\n- File: "		<< Location.file_name()
				<< "\n- Line: "		<< Location.line()
				<< "\n- Function: "	<< Location.function_name();
			return ss.str();
		}

		explicit Signal(
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