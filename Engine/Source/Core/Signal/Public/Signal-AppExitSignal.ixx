module;
#include <Visera.h>

export module Visera.Core.Signal:AppExitSignal;
import :Common;

export namespace VE
{
	
	class AppExitSignal final : public Signal
	{
	public:
		explicit AppExitSignal(
			const String& Message,
			const std::source_location& Location = std::source_location::current())
			:
			Signal{Message, Location}
		{

		}
	};

VISERA_MODULE_END