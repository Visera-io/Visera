#pragma once

namespace VE
{
    template<typename T>
    class Singleton
    {
    protected:
        Singleton() = default;
        virtual ~Singleton() noexcept = default;

    public:
        static T& GetInstance() noexcept(std::is_nothrow_constructible<T>::value)
        {
            static T singleton{};
            return singleton;
        }

    protected:
        Singleton(const Singleton&)             = delete;
        Singleton(Singleton&&)                  = delete;
        Singleton& operator=(const Singleton&)  = delete;
        Singleton& operator=(Singleton&&)       = delete;
    };
} // namespace VE