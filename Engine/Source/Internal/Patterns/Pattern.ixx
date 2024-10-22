module;
#include <ViseraEngine>

export module Visera.Internal.Pattern;
export import :Singleton;

export namespace VE
{
    //[Concepts]
    template<typename T>
    concept SingletonClass = std::is_class_v<Singleton<T>> || std::is_base_of_v<Singleton<T>, T>;
 
} // namespace VE