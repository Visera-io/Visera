module;
#include <Visera.h>
export module Visera.Core.Signal:AppStop;
import :Interface;

export namespace VE
{

	class SAppStop final : public ISignal
	{
	public:
		SAppStop(
			const String& _Message, Int32 _StateCode = 0,
			const std::source_location& _Location = std::source_location::current())
			:
			ISignal{_Message, _StateCode, _Location}
		{

		}
	};

} // namespace VE