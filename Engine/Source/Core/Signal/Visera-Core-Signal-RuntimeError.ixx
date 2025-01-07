module;
#include <Visera.h>

export module Visera.Core.Signal:RuntimeError;
import :Interface;

VISERA_PUBLIC_MODULE
	
class RuntimeError final : public Signal
{
public:
	explicit RuntimeError(
		const String& Message,
		const std::source_location& Location = std::source_location::current())
		:
		Signal{Message, Location}
	{

	}
};

VISERA_MODULE_END