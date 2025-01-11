module;
#include <Visera.h>

export module Visera.Core.Time:Duration;

export namespace VE
{


template<ClockType T>
class TDuration
{
public:
    auto nanoseconds()  const { return std::chrono::duration_cast<std::chrono::nanoseconds>(Value).count(); }
    auto microseconds() const { return std::chrono::duration_cast<std::chrono::microseconds>(Value).count(); }
    auto milliseconds() const { return std::chrono::duration_cast<std::chrono::milliseconds>(Value).count(); }
    auto seconds()      const { return std::chrono::duration_cast<std::chrono::seconds>(Value).count(); }
    auto minutes()      const { return std::chrono::duration_cast<std::chrono::minutes>(Value).count(); }
    auto hours()        const { return std::chrono::duration_cast<std::chrono::hours>(Value).count(); }
    auto days()         const { return std::chrono::duration_cast<std::chrono::days>(Value).count(); }
public:
    TDuration() noexcept : Value{ 0 } {};
    TDuration(const TDuration<T>& NewDuration) :Value{ NewDuration.Value } {}
    TDuration(TDuration<T>&& SourceDuration)      :Value{ std::move(SourceDuration.Value) } {}
    TDuration(const T::duration& NewDuration) :Value{ NewDuration } {}
    TDuration(T::duration&& SourceDuration)      :Value{ std::move(SourceDuration) } {}
    TDuration(T::time_point Start, T::time_point End) :Value{ End - Start } {}
    //TDuration<T>& operator=(const T::duration& TDuration) { Value = TDuration; return *this; }
    TDuration<T>& operator=(T::duration&& SourceDuration)      { Value = SourceDuration; return *this; }

    TDuration<T>  operator+ (const TDuration<T>&  Addend)     const  { return {Value + Addend.Value}; }
    TDuration<T>& operator+=(const TDuration<T>&  Addend)            { Value += Addend.Value; return *this; }
    TDuration<T>  operator- (const TDuration<T>&  Subtrahend) const  { return {Value - Subtrahend.Value}; }
    TDuration<T>& operator-=(const TDuration<T>&  Subtrahend)        { Value -= Subtrahend.Value; return *this; }
private:
    T::duration Value;
};

} // namespace VE