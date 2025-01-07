module;
#include <Visera.h>

export module Visera.Core.Signal:AppStopSignal;
import :Interface;

VISERA_PUBLIC_MODULE
class AppStopSignal final : public Signal
{
public:
	explicit AppStopSignal(
		const String& Message,
		const std::source_location& Location = std::source_location::current())
		:
		Signal{Message, Location}
	{

	}
};

VISERA_MODULE_END