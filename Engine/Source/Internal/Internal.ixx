module;
#include <ViseraEngine>

export module Visera.Internal;
export import Visera.Internal.Memory;
export import Visera.Internal.Pattern;

import Visera.Core.Log;

export namespace VE
{
	class Visera;

	class ViseraInternal
	{
		friend class Visera;
	public:
		struct RuntimeContext
		{
			struct
			{
				enum State: EnumBit
				{
					Running  = 0,
					Stopping = 1 << 0, 
				};
				EnumMask States = Running;
			
				void Stop(const String& Signature) { States |= Stopping; Log::Warn("{} is stopping Visera Main Loop...", Signature); }
				Bool ShouldStop() const { return States & Stopping; }
			}MainLoop;
		};
		static inline RuntimeContext Context;

	private:
		static inline void
		Tick()
		{
			if (!Context.MainLoop.ShouldStop())
			{

			}
		}

		static inline void
		Bootstrap()
		{
			Context.MainLoop.States = Context.MainLoop.Running;
		}

		static inline void
		Terminate()
		{
			
		}

		ViseraInternal() noexcept = default;
	};

} // namespace VE