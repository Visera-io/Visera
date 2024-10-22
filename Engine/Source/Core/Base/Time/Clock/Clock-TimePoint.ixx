module;
#include <ViseraEngine>

export module Visera.Core.Base.Time.Clock:TimePoint;
import :Duration;

namespace
{
    template<typename T>
    concept ClockType = std::is_class_v<std::chrono::system_clock>          ||
                        std::is_class_v<std::chrono::high_resolution_clock>;
}

export namespace VE
{	

    template<ClockType T>
    class TimePoint
    {
    public:
        TimePoint() = default;
        TimePoint(T::time_point timepoint) : m_timepoint{ std::move(timepoint) }{}

        operator T::time_point() const { return m_timepoint; }
        auto&       operator=(const TimePoint<T>& target) { m_timepoint = target.m_timepoint; return *this; }
        Duration<T> operator-(TimePoint<T> target) const { return {m_timepoint - target.m_timepoint}; }
    private:
        T::time_point m_timepoint;
    };

    template<>
    class TimePoint<std::chrono::system_clock>
    {
    public:
        //[FIXME] Time Zone issues.
        std::string
        ToString() const
        { return std::format("UTC(+0) {:%Y-%m-%d %H:%M:%S}", m_timepoint); }

    public:
        TimePoint() = default; //UNIX Time
        TimePoint(std::chrono::system_clock::time_point timepoint) : m_timepoint{ std::move(timepoint) }{}
        
        operator std::chrono::system_clock::time_point() const { return m_timepoint; }
        TimePoint<std::chrono::system_clock>& operator=(const TimePoint<std::chrono::system_clock>& target) { m_timepoint = target.m_timepoint; return *this; }
        Duration<std::chrono::system_clock> operator-(TimePoint<std::chrono::system_clock> target) const { return {m_timepoint - target.m_timepoint}; }
    private:
        std::chrono::system_clock::time_point m_timepoint;
    };

} // namespace VE