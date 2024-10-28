module;
#include <ViseraEngine>

export module Visera.Core.Time.Clock:Duration;

namespace
{
    template<typename T>
    concept ClockType = std::is_class_v<std::chrono::system_clock>          ||
                        std::is_class_v<std::chrono::high_resolution_clock>;
}

export namespace VE
{	
    template<ClockType T>
	class Duration
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
        Duration() noexcept : Value{ 0 } {};
        Duration(const Duration<T>& NewDuration) :Value{ NewDuration.Value } {}
        Duration(Duration<T>&& SourceDuration)      :Value{ std::move(SourceDuration.Value) } {}
        //Duration(const T::duration& duration) :Value{ duration } {}
        Duration(T::duration&& SourceDuration)      :Value{ std::move(SourceDuration) } {}
        Duration(T::time_point Start, T::time_point End) :Value{ End - Start } {}
        //Duration<T>& operator=(const T::duration& duration) { Value = duration; return *this; }
        Duration<T>& operator=(T::duration&& SourceDuration)      { Value = SourceDuration; return *this; }

        Duration<T>  operator+(Duration<T> Addend)  { return {Value + Addend.Value}; }
        Duration<T>& operator+=(Duration<T> Addend) { Value += Addend.Value; return *this; }
        Duration<T>  operator-(Duration<T> Subtrahend)  { return {Value - Subtrahend.Value}; }
        Duration<T>& operator-=(Duration<T> Subtrahend) { Value -= Subtrahend.Value; return *this; }

        operator T() const { return Value; }
    private:
        T::duration Value;
    };

} // namespace VE