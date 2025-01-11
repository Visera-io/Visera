module;
#include <Visera.h>
export module Visera.Core.Time:TimePoint;

import :Duration;

export namespace VE
{

    template<ClockType T>
    class TTimePoint
    {
    public:
        TTimePoint() = default;
        TTimePoint(const T::time_point& _NewTimePoint) : Value{ _NewTimePoint }{}
        TTimePoint(T::time_point&& _NewTimePoint) : Value{ std::move(_NewTimePoint) }{}

        operator T::time_point() const { return Value; }
    private:
        T::time_point Value;
    };

    template<>
    class TTimePoint<std::chrono::high_resolution_clock>
    {
    public:
        TTimePoint() = default;
        TTimePoint(const std::chrono::high_resolution_clock::time_point& TTimePoint) : Value{ TTimePoint }{}
        TTimePoint(std::chrono::high_resolution_clock::time_point&& TTimePoint) : Value{ std::move(TTimePoint) }{}

        operator std::chrono::high_resolution_clock::time_point() const { return Value; }
        auto&   operator=(const TTimePoint<std::chrono::high_resolution_clock>& target) { Value = target.Value; return *this; }
        auto&   operator=(TTimePoint<std::chrono::high_resolution_clock>&& target)      { Value = std::move(target.Value); return *this; }
        auto    operator-(const std::chrono::high_resolution_clock::time_point& target) const { return TDuration<std::chrono::high_resolution_clock>{ Value - target }; }
        auto    operator-(const TTimePoint<std::chrono::high_resolution_clock>& target)  const { return TDuration<std::chrono::high_resolution_clock>{ Value - target.Value }; }
    private:
        std::chrono::high_resolution_clock::time_point Value;
    };

    template<>
    class TTimePoint<std::chrono::system_clock>
    {
    public:
        //[TODO] Time Zone issues.
        String ToString() const
        { return Text("UTC(+0) {:%Y-%m-%d %H:%M:%S}", Value); }

    public:
        TTimePoint() = default; //UNIX Time
        TTimePoint(std::chrono::system_clock::time_point Value) : Value{ std::move(Value) }{}
        
        operator std::chrono::system_clock::time_point() const { return Value; }
        auto&   operator=(const TTimePoint<std::chrono::system_clock>& target) { Value = target.Value; return *this; }
        auto&   operator=(TTimePoint<std::chrono::system_clock>&& target)      { Value = std::move(target.Value); return *this; }
        auto    operator-(const std::chrono::system_clock::time_point& target) const { return TDuration<std::chrono::system_clock>{ Value - target }; }
        auto    operator-(const TTimePoint<std::chrono::system_clock>& target)  const { return TDuration<std::chrono::system_clock>{ Value - target.Value }; }
    private:
        std::chrono::system_clock::time_point Value;
    };

} // namespace VE