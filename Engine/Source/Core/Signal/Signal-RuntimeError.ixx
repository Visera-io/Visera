module;
#include <Visera.h>

export module Visera.Core.Signal:RuntimeError;
import :Common;

export namespace VE
{
	
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

} // namespace VE