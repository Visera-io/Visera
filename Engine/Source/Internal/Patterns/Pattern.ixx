module;
#include <Visera.h>

export module Visera.Internal.Pattern;
export import :Singleton;

VISERA_PUBLIC_MODULE
    //[Concepts]
    template<typename T>
    concept SingletonClass = std::is_class_v<Singleton<T>> || std::is_base_of_v<Singleton<T>, T>;
 
VISERA_MODULE_END