module;
#include <Visera.h>

export module Visera.Core.Signal:IOFailure;
import :Interface;

export namespace VE
{
	
	class SIOFailure final : public ISignal
	{
	public:
		SIOFailure(
			StringView _Message, Int32 _StateCode = 0,
			const std::source_location& Location = std::source_location::current())
			:
			ISignal{_Message, _StateCode, Location}
		{

		}
	};

} // namespace VE