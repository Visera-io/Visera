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

		//[FIXME]: Unknown Bug (Perhaps, a MSVC bug: https://github.com/microsoft/STL/issues/5203)
		VE_API Sleep(UInt32 _MilliSeconds) { std::this_thread::sleep_for(std::chrono::milliseconds(_MilliSeconds)); }

		VE_API Now()				{ return SystemClock.Now(); }
		VE_API GetRunningTime()		{ return SystemClock.GetTotalTime(); }

		VE_API IsLittleEndian()		{ return std::endian::native == std::endian::little; }
		VE_API IsWindowsSystem()	{ return VE_IS_WINDOWS_SYSTEM; }
		VE_API IsX86CPU()			{ return VE_IS_X86_CPU; }
		VE_API IsArmCPU()			{ return VE_IS_ARM_CPU; }
		
		VE_API constexpr CacheLineSize = 64U;

		static inline FSystemClock	SystemClock;
	};

} // namespace VE