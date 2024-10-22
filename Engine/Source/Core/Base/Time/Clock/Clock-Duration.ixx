module;
#include <ViseraEngine>

export module Visera.Core.Base.Time.Clock:Duration;

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
        auto nanoseconds()  const { return std::chrono::duration_cast<std::chrono::nanoseconds>(m_duration).count(); }
        auto microseconds() const { return std::chrono::duration_cast<std::chrono::microseconds>(m_duration).count(); }
        auto milliseconds() const { return std::chrono::duration_cast<std::chrono::milliseconds>(m_duration).count(); }
        auto seconds()      const { return std::chrono::duration_cast<std::chrono::seconds>(m_duration).count(); }
        auto minutes()      const { return std::chrono::duration_cast<std::chrono::minutes>(m_duration).count(); }
        auto hours()        const { return std::chrono::duration_cast<std::chrono::hours>(m_duration).count(); }
        auto days()         const { return std::chrono::duration_cast<std::chrono::days>(m_duration).count(); }
    public:
        Duration() noexcept : m_duration{ 0 } {};
        Duration(const Duration<T>& duration) :m_duration{ duration.m_duration } {}
        Duration(Duration<T>&& duration)      :m_duration{ std::move(duration.m_duration) } {}
        //Duration(const T::duration& duration) :m_duration{ duration } {}
        Duration(T::duration&& duration)      :m_duration{ std::move(duration) } {}
        Duration(T::time_point start, T::time_point end) :m_duration{ end - start } {}
        //Duration<T>& operator=(const T::duration& duration) { m_duration = duration; return *this; }
        Duration<T>& operator=(T::duration&& duration)      { m_duration = duration; return *this; }

        Duration<T>  operator+(Duration<T> target)  { return {m_duration + target.m_duration}; }
        Duration<T>& operator+=(Duration<T> target) { m_duration += target.m_duration; return *this; }
        Duration<T>  operator-(Duration<T> target)  { return {m_duration - target.m_duration}; }
        Duration<T>& operator-=(Duration<T> target) { m_duration -= target.m_duration; return *this; }

        operator T() const { return m_duration; }
    private:
        T::duration m_duration;
    };

} // namespace VE