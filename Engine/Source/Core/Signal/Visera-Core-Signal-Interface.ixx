module;
#include <Visera.h>

export module Visera.Core.Signal:Interface;

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
		const String& Message, Int32 _StateCode,
		const std::source_location& Location = std::source_location::current())
		:
		Message		{ std::move(Message) },
		Location	{ std::move(Location) },
		StateCode	{_StateCode}
	{

	}
	
	const String Message;
	const std::source_location Location;
	const Int32 StateCode;
};

} // namespace VE