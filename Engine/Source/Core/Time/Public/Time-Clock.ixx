module;
#include <Visera.h>

export module Visera.Core.Time:Clock;
import :Duration;
import :TimePoint;
import :TimeZone;

//[Interface]	Clock
//[Class]		HiResClock
//[Class]		SystemClock

VISERA_PRIVATE_MODULE
template<ClockType T>
class Clock
{
public:
	static inline
    TimePoint<T>
    Now() { return { T::now() }; }

	inline
    Duration<T>
	Tick() { auto OldTime = LastTickTimePoint; LastTickTimePoint = Now(); return { LastTickTimePoint - OldTime }; }

	inline
    Duration<T>
	Elapsed() const { return Duration<T>{ Now() - LastTickTimePoint }; }

	inline
    Duration<T>
	GetTotalTime() const { return Duration<T>{ Now() - StartTimePoint }; }

    inline void
    Set(TimePoint<T> NewTimePoint) { LastTickTimePoint = NewTimePoint; }

	inline void
	Reset() { LastTickTimePoint = TimePoint<T>{}; StartTimePoint = LastTickTimePoint;}

public:
	Clock() noexcept : StartTimePoint{ Now() }, LastTickTimePoint{ Now() } {}

protected:
	TimePoint<T> StartTimePoint;
	TimePoint<T> LastTickTimePoint;
};
VISERA_MODULE_END

VISERA_PUBLIC_MODULE
class HiResClock  : public Clock<std::chrono::high_resolution_clock> {};
class SystemClock : public Clock<std::chrono::system_clock> {};
VISERA_MODULE_END