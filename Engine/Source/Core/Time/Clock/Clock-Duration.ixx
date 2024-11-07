module;
#include <Visera>

export module Visera.Engine.Core.Time.Clock:Duration;

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
        //Duration(T::duration duration) :Value{ duration } {}
        Duration(T::duration&& SourceDuration)      :Value{ std::move(SourceDuration) } {}
        Duration(T::time_point Start, T::time_point End) :Value{ End - Start } {}
        //Duration<T>& operator=(const T::duration& duration) { Value = duration; return *this; }
        Duration<T>& operator=(T::duration&& SourceDuration)      { Value = SourceDuration; return *this; }

        Duration<T>  operator+ (const Duration<T>&  Addend)     const  { return {Value + Addend.Value}; }
        Duration<T>& operator+=(const Duration<T>&  Addend)            { Value += Addend.Value; return *this; }
        Duration<T>  operator- (const Duration<T>&  Subtrahend) const  { return {Value - Subtrahend.Value}; }
        Duration<T>& operator-=(const Duration<T>&  Subtrahend)        { Value -= Subtrahend.Value; return *this; }
    private:
        T::duration Value;
    };

} // namespace VE