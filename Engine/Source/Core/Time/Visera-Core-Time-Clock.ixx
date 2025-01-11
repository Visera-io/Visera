module;
#include <Visera.h>

export module Visera.Core.Time:Clock;
import :Duration;
import :TimePoint;
import :TimeZone;

//[Interface]	TClock
//[Class]		FHiResClock
//[Class]		FSystemClock

namespace VE
{

	template<ClockType T>
	class TClock
	{
	public:
		static inline
		TTimePoint<T>
		Now() { return { T::now() }; }

		inline
		TDuration<T>
		Tick() { auto OldTime = LastTickTimePoint; LastTickTimePoint = Now(); return { LastTickTimePoint - OldTime }; }

		inline
		TDuration<T>
		Elapsed() const { return TDuration<T>{ Now() - LastTickTimePoint }; }

		inline
		TDuration<T>
		GetTotalTime() const { return TDuration<T>{ Now() - StartTimePoint }; }

		inline void
		Set(TTimePoint<T> NewTimePoint) { LastTickTimePoint = NewTimePoint; }

		inline void
		Reset() { LastTickTimePoint = TTimePoint<T>{}; StartTimePoint = LastTickTimePoint;}

	public:
		TClock() noexcept : StartTimePoint{ Now() }, LastTickTimePoint{ Now() } {}

	protected:
		TTimePoint<T> StartTimePoint;
		TTimePoint<T> LastTickTimePoint;
	};

} // namespace VE

export namespace VE
{
	class FHiResClock  : public TClock<std::chrono::high_resolution_clock> {};
	class FSystemClock : public TClock<std::chrono::system_clock> {};
} // namespace VE