module;
#include <Visera.h>

export module Visera.Core.Time.Clock;
export import :Duration;
export import :TimePoint;
export import :TimeZone;

export namespace VE
{	
	//[Interface]	Clock
	//[Class]		HiResClock
	//[Class]		SystemClock

	template<ClockType T>
	class Clock
	{
	public:
		static inline
        TimePoint<T>
        Now() { return { T::now() }; }

		inline
        Duration<T>
		Tick() { auto OldTime = CurrentTimePoint; CurrentTimePoint = Now(); Duration<T> delta{ CurrentTimePoint - OldTime }; CurrentDuration += delta;  return delta; }

		inline
        Duration<T>
		Elapsed() const { return Duration<T>{ Now() - CurrentTimePoint }; }

		inline
        Duration<T>
		GetTotalTime() const { return Duration<T>{ Now() - CurrentTimePoint } + CurrentDuration; }

        inline void
        Set(TimePoint<T> NewTimePoint) { CurrentTimePoint = NewTimePoint; }

		inline void
		Reset() { CurrentTimePoint = TimePoint<T>{}; CurrentDuration = Duration<T>{};}

    public:
        Clock() noexcept : CurrentTimePoint{ Now() } {}

	protected:
		TimePoint<T> CurrentTimePoint;
		Duration<T>  CurrentDuration;
	};

	class HiResClock : public Clock<std::chrono::high_resolution_clock> {};
	class SystemClock : public Clock<std::chrono::system_clock> {};

} // namespace VE