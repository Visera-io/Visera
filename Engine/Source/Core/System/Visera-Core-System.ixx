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
	public:
		//[FIXME]: Unknown Bug (Perhaps, a MSVC bug: https://github.com/microsoft/STL/issues/5203)
		/*static inline void
		Sleep(UInt32 _MilliSeconds) { std::this_thread::sleep_for(std::chrono::milliseconds(_MilliSeconds)); }*/

		static inline auto
		Now() { return SystemClock.Now(); }
		static inline auto
		GetRunningTime() { return SystemClock.GetTotalTime(); }
		constexpr Bool
		IsLittleEndian() { return std::endian::native == std::endian::little; }

	private:
		static inline FSystemClock	SystemClock;
	};

} // namespace VE