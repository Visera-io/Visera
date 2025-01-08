module;
#include <Visera.h>

export module Visera.Core.Signal:AppStopSignal;
import :Interface;

VISERA_PUBLIC_MODULE
class AppStopSignal final : public Signal
{
public:
	explicit AppStopSignal(
		const String& _Message, Int32 _StateCode = 0,
		const std::source_location& _Location = std::source_location::current())
		:
		Signal{_Message, _StateCode, _Location}
	{

	}
};

VISERA_MODULE_END