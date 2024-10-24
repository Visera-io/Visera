module;
#include <ViseraEngine>

export module Visera.Core.Time.Clock;
export import :Duration;
export import :TimePoint;
export import :TimeZone;

export namespace VE
{	
	//[Interface]	Clock
	//[Class]		HiResClock
	//[Class]		SystemClock

	template<typename T>
    concept ClockType = std::is_class_v<std::chrono::system_clock>          ||
                        std::is_class_v<std::chrono::high_resolution_clock>;

	template<ClockType T>
	class Clock
	{
	public:
		static inline
        TimePoint<T>
        Now() { return { T::now() }; }

		inline
        Duration<T>
		Tick() { auto old_time = m_time; m_time = Now(); Duration<T> delta{ m_time - old_time }; m_duration += delta;  return delta; }

		inline
        Duration<T>
		Elapsed() const { return Duration<T>{ Now() - m_time }; }

		inline
        Duration<T>
		TotalTime() const { return Duration<T>{ Now() - m_time } + m_duration; }

        inline void
        Set(TimePoint<T> time) { m_time = time; }

		inline void
		Reset() { m_time = TimePoint<T>{}; m_duration = Duration<T>{};}

    public:
        Clock() noexcept : m_time{ Now() } {}

	protected:
		TimePoint<T> m_time;
		Duration<T>  m_duration;
	};

	class HiResClock : public Clock<std::chrono::high_resolution_clock> {};
	class SystemClock : public Clock<std::chrono::system_clock> {};

} // namespace VE