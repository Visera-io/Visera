module;
#include <Visera>

export module Visera.Engine.Core.Signal:AppExitSignal;
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

} // namespace VE