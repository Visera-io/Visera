module;
#include <Visera.h>

export module Visera.Core.Signal:RuntimeError;
import :Interface;

export namespace VE
{

	class SRuntimeError final : public ISignal
	{
	public:
		SRuntimeError(
			StringView _Message, Int32 _StateCode = 0,
			const std::source_location& Location = std::source_location::current())
			:
			ISignal{_Message, _StateCode, Location}
		{

		}
	};

} // namespace VE