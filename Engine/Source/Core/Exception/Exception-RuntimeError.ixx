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
			const std::string& message,
			const std::source_location& location = std::source_location::current())
			:
			Error{message, location}
		{

		}
	};

} // namespace VE