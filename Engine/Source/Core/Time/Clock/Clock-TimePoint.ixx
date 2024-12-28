module;
#include <Visera.h>

export module Visera.Core.Time.Clock:TimePoint;
import :Duration;

export namespace VE
{	

    template<ClockType T>
    class TimePoint
    {
    public:
        TimePoint() = default;
        TimePoint(T::time_point timepoint) : Value{ std::move(timepoint) }{}

        operator T::time_point() const { return Value; }
        auto&       operator=(const TimePoint<T>& target) { Value = target.Value; return *this; }
        Duration<T> operator-(const TimePoint<T>& target) const { return { Value - target.Value }; }
    private:
        T::time_point Value;
    };

    template<>
    class TimePoint<std::chrono::system_clock>
    {
    public:
        //[FIXME] Time Zone issues.
        std::string
        ToString() const
        { return Text("UTC(+0) {:%Y-%m-%d %H:%M:%S}", Value); }

    public:
        TimePoint() = default; //UNIX Time
        TimePoint(std::chrono::system_clock::time_point Value) : Value{ std::move(Value) }{}
        
        operator std::chrono::system_clock::time_point() const { return Value; }
        TimePoint<std::chrono::system_clock>& operator=(const TimePoint<std::chrono::system_clock>& NewTimePoint) { Value = NewTimePoint.Value; return *this; }
        Duration<std::chrono::system_clock> operator-(TimePoint<std::chrono::system_clock> Subtrahend) const { return { Value - Subtrahend.Value }; }
    private:
        std::chrono::system_clock::time_point Value;
    };

VISERA_MODULE_END