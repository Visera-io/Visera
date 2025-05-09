module;
#include <Visera.h>
export module Visera.Core.OS;
export import Visera.Core.OS.Memory;
export import Visera.Core.OS.FileSystem;
export import Visera.Core.OS.Concurrency;

import Visera.Core.Time;

export namespace VE
{
	class OS
	{
		VE_MODULE_MANAGER_CLASS(OS);

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