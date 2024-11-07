module;
#include <Visera>

export module Visera.App.Base;

export namespace VISERA_APP_NAMESPACE
{
	inline void Assert(bool expression, const char* prompt = "") { assert(expression && prompt); }

    constexpr bool False   = false;
    constexpr bool True    = !False;

    /* <<  Basic Types >>
       
    */
    using Bool		= VE::Bool;
    using Float  	= VE::Float;
    using Double 	= VE::Double;
    using Int32  	= VE::Int32;
    using UInt32 	= VE::UInt32;
    using Int64  	= VE::Int64;
    using UInt64 	= VE::UInt64;

    using String	 = VE::String;
    using StringView = VE::StringView;
    using RawString  = VE::RawString;

    using Byte		= VE::Byte;
    using EnumMask	= VE::EnumMask;
    using EnumBit	= VE::EnumBit;
    template<typename T> inline
    UInt64 BytesOf() { return sizeof(T); }
    template<typename T> inline
    UInt64 BitsOf()  { return 8 * BytesOf<T>(); }

    /* <<  Containers >>
        1. Array
        2. Set
        3. HashMap
    */

    template<typename T>
    using Array	   = VE::Array<T>;

    template<typename T>
    using Set	   = VE::Set<T>;

    template<VE::Hashable Key, typename Value>
    using HashMap  = VE::HashMap<Key, Value>;

    /* <<  Pointers >>
        1. SharedPtr
        2. WeakPtr
        3. UniquePtr
    */

    template<typename T>
    using SharedPtr   = VE::SharedPtr<T>;
    template<typename T, typename... Args>
    inline SharedPtr<T>
    CreateSharedPtr(Args &&...args) { return VE::CreateSharedPtr<T>(std::forward<Args>(args)...); }

    template<typename T>
    using WeakPtr	  = VE::WeakPtr<T>;

    template<typename T>
    using UniquePtr   = VE::UniquePtr<T>;
    template<typename T, typename... Args>
    inline UniquePtr<T> 
    CreateUniquePtr(Args &&...args) { return VE::CreateUniquePtr<T>(std::forward<Args>(args)...); }

    template <typename T>
    concept Hashable = requires(T type) {{ std::hash<T>{}(type) } -> std::same_as<std::size_t>;};

    template<typename T>
    concept UnsignedIntegerol = std::unsigned_integral<T>;

    template<typename T>
    concept SignedInteger = std::integral<T>;

    template<typename T>
    concept Integer = std::integral<T> || std::unsigned_integral<T>;

    template<typename T>
    concept FloatingPoint = std::floating_point<T>;

    template<typename T>
    concept Number = std::floating_point<T> || std::integral<T>;

} // namespace VISERA_APP_NAMESPACE