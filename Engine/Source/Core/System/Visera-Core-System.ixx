module;
#include <Visera.h>
export module Visera.Core.System;

export import Visera.Core.System.Memory;
export import Visera.Core.System.FileSystem;
export import Visera.Core.System.Concurrency;

import Visera.Core.Time;

export namespace VE
{
	class System
	{
		VE_MODULE_MANAGER_CLASS(System);
	public:
		//[FIXME]: Unknown Bug (Perhaps, a MSVC bug: https://github.com/microsoft/STL/issues/5203)
		/*VE_API Sleep(UInt32 _MilliSeconds) { std::this_thread::sleep_for(std::chrono::milliseconds(_MilliSeconds)); }*/

		VE_API Now()			{ return SystemClock.Now(); }
		VE_API GetRunningTime() { return SystemClock.GetTotalTime(); }
		VE_API IsLittleEndian() { return std::endian::native == std::endian::little; }

	private:
		static inline FSystemClock	SystemClock;
	};

} // namespace VE