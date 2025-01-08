module;
#include <Visera.h>

export module Visera.Core.Signal:EngineStopSignal;
import :Interface;

VISERA_PUBLIC_MODULE

class EngineStopSignal final : public Signal
{
public:
	explicit EngineStopSignal(
		const String& Message, Int32 _StateCode = 0,
		const std::source_location& Location = std::source_location::current())
		:
		Signal{Message, _StateCode, Location}
	{

	}
};
VISERA_MODULE_END