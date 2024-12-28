module;
#include <Visera.h>

export module Visera.Internal.Pattern:Singleton;

export namespace VE
{
    /*[USAGE]
    * SINGLETON : public Singleton<?>
    * { 
    *   friend class Singleton<SINGLETON>; 
    *   SINGLETON() = default; // Make Constructor private.
    * };
    */
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
VISERA_MODULE_END