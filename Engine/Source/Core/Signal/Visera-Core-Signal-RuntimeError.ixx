module;
#include <Visera.h>

export module Visera.Core.Signal:RuntimeError;
import :Interface;

export namespace VE
{

	
class RuntimeError final : public Signal
{
public:
	explicit RuntimeError(
		const String& Message, Int32 _StateCode = 0,
		const std::source_location& Location = std::source_location::current())
		:
		Signal{Message, _StateCode, Location}
	{

	}
};

} // namespace VE