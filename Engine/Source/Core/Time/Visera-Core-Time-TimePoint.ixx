module;
#include <Visera.h>

export module Visera.Core.Time:TimePoint;
import :Duration;

export namespace VE
{


template<ClockType T>
class TimePoint
{
public:
    TimePoint() = default;
    TimePoint(const T::time_point& timepoint) : Value{ timepoint }{}
    TimePoint(T::time_point&& timepoint) : Value{ std::move(timepoint) }{}

    operator T::time_point() const { return Value; }
private:
    T::time_point Value;
};

template<>
class TimePoint<std::chrono::high_resolution_clock>
{
public:
    TimePoint() = default;
    TimePoint(const std::chrono::high_resolution_clock::time_point& timepoint) : Value{ timepoint }{}
    TimePoint(std::chrono::high_resolution_clock::time_point&& timepoint) : Value{ std::move(timepoint) }{}

    operator std::chrono::high_resolution_clock::time_point() const { return Value; }
    auto&   operator=(const TimePoint<std::chrono::high_resolution_clock>& target) { Value = target.Value; return *this; }
    auto&   operator=(TimePoint<std::chrono::high_resolution_clock>&& target)      { Value = std::move(target.Value); return *this; }
    auto    operator-(const std::chrono::high_resolution_clock::time_point& target) const { return Duration<std::chrono::high_resolution_clock>{ Value - target }; }
    auto    operator-(const TimePoint<std::chrono::high_resolution_clock>& target)  const { return Duration<std::chrono::high_resolution_clock>{ Value - target.Value }; }
private:
    std::chrono::high_resolution_clock::time_point Value;
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
    auto&   operator=(const TimePoint<std::chrono::system_clock>& target) { Value = target.Value; return *this; }
    auto&   operator=(TimePoint<std::chrono::system_clock>&& target)      { Value = std::move(target.Value); return *this; }
    auto    operator-(const std::chrono::system_clock::time_point& target) const { return Duration<std::chrono::system_clock>{ Value - target }; }
    auto    operator-(const TimePoint<std::chrono::system_clock>& target)  const { return Duration<std::chrono::system_clock>{ Value - target.Value }; }
private:
    std::chrono::system_clock::time_point Value;
};

} // namespace VE