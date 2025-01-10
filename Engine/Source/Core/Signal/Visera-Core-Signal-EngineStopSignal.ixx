module;
#include <Visera.h>

export module Visera.Core.Signal:EngineStopSignal;
import :Interface;

export namespace VE
{


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
} // namespace VE