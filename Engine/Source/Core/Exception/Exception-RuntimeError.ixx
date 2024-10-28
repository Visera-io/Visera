module;
#include <ViseraEngine>

export module Visera.Core.Exception:RuntimeError;
import :Error;

export namespace VE
{
	
	class RuntimeError final : public Error
	{
	public:
		explicit RuntimeError(
			const String& Message,
			const std::source_location& Location = std::source_location::current())
			:
			Error{Message, Location}
		{

		}
	};

} // namespace VE