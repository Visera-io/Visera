module;
#include <Visera.h>

export module Visera.Core.Signal:RuntimeError;
import :Interface;

VISERA_PUBLIC_MODULE
	
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

VISERA_MODULE_END