module;
#include <ViseraEngine>

export module Visera.Runtime.Context;

import Visera.Core.Log;

export namespace VE
{
	class ViseraRuntime;

	class RuntimeContext
	{
		friend class ViseraRuntime;
	public:
		static inline struct
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

	private:
		static inline void
		Bootstrap()
		{

		}

		static inline void
		Terminate()
		{

		}
	};
}