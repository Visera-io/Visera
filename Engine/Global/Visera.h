#pragma once

#define VISERA_YEAR     2024
#define VISERA_AUTHOR   "LJYC (ljyc.me)"

// << Global Macros >>
#define VISERA_ENGINE_ERROR -1
#define VISERA_APP_ERROR    -2

#define VE_ASSERT(Expression)	assert(Expression);
#define VE_API public: static inline auto
#define VE_WIP assert(false && "Work In Progress...")
#define VE_PATH(Path) VE::PathStringView(L##Path)

#define VE_REGISTER_AUTOCAST(IType, OType) constexpr OType AutoCast(IType src) { return static_cast<OType>(src); }

#define VE_OP_BITOR(IType)	constexpr IType operator|(IType lhs, IType rhs) { return static_cast<IType>(static_cast<EnumBit>(lhs) | static_cast<EnumBit>(rhs)); }
#define VE_OP_BITXOR(IType)	constexpr IType operator^(IType lhs, IType rhs) { return static_cast<IType>(static_cast<EnumBit>(lhs) ^ static_cast<EnumBit>(rhs)); }
#define VE_OP_BITAND(IType)	constexpr IType operator&(IType lhs, IType rhs) { return static_cast<IType>(static_cast<EnumBit>(lhs) & static_cast<EnumBit>(rhs)); }

#define VE_ENUM_CLASS(Name, Super)		\
		enum class Name : EnumBit;		\
		VE_REGISTER_AUTOCAST(Name,Super)\
		VE_OP_BITOR(Name)		        \
		VE_OP_BITXOR(Name)		        \
		VE_OP_BITAND(Name);	            \
		enum class Name : EnumBit

#define VE_NOT_COPYABLE(ClassName) \
	ClassName(const ClassName&) = delete;\
	ClassName& operator=(const ClassName&) = delete;

#define VE_NOT_MOVABLE(ClassName) \
	ClassName(ClassName&&) = delete;\
	ClassName& operator=(ClassName&&) = delete;

#define VE_MODULE_MANAGER_CLASS(ClassName)\
    ClassName() noexcept = delete;\
    VE_NOT_COPYABLE(ClassName);\
    VE_NOT_MOVABLE(ClassName);

#if defined(_WIN32) || defined(_WIN64)
#define VE_IS_WINDOWS_SYSTEM true
#define VE_ON_WINDOWS_SYSTEM
#else
#define VE_IS_WINDOWS_SYSTEM false
#endif
#if defined(__APPLE__)
#define VE_IS_APPLE_SYSTEM true
#define VE_ON_APPLE_SYSTEM
#else
#define VE_IS_APPLE_SYSTEM false
#endif
#if (defined(_M_IX86) || defined(__i386__) || defined(_M_X64) || defined(__amd64__) || defined(__x86_64__)) && !defined(_M_ARM64EC)
#define VE_IS_X86_CPU true
#define VE_ON_X86_CPU
#else
#define VE_IS_X86_CPU false
#endif
#if (defined(__arm__) || defined(_M_ARM) || defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC))
#define VE_IS_ARM_CPU true
#define VE_ON_ARM_CPU
#else
#define VE_IS_ARM_CPU false
#endif
// << STD Modules >>
#include <cassert>
#include <sstream>
#include <ostream>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <format>
#include <vector>
#include <list>
#include <bit>
#include <algorithm>
#include <array>
#include <bitset>
#include <shared_mutex>
#include <ranges>
#include <memory>
#include <typeinfo>
#include <filesystem>
#include <functional>
#include <source_location>
#include <exception>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <type_traits>

namespace VE
{

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

	template<typename T>
	concept Pointer = std::is_pointer_v<T>;

	template<typename T>
	concept Alignable = std::integral<T> || std::is_pointer_v<T>;

	template<typename T>
	concept ClockType = std::is_class_v<std::chrono::system_clock>          ||
	                    std::is_class_v<std::chrono::high_resolution_clock>;

	/* <<  Basic Types >>

	*/
	using Bool		= bool;
	using Float  	= float;
	using Double 	= double;
	using Int8      = char;
	using UInt8     = unsigned char;
	using Int16     = int16_t;
	using UInt16    = uint16_t;
	using Int32  	= std::int32_t;
	using UInt32 	= std::uint32_t;
	using Int64  	= std::int64_t;
	using UInt64 	= std::uint64_t;

	using ANSIChar   = char;
	using WideChar   = wchar_t;
	using String	 = std::string;
	using StringView = std::string_view;
	using RawString  = const char*;

	template <class... _Types> inline
	String
	Text(const std::format_string<_Types...> _Fmt, _Types&&... _Args)
	{ return std::format(_Fmt, std::forward<_Types>(_Args)...); }

	template<unsigned int _Size>
	using Bits      = std::bitset<_Size>;
	using Byte		= unsigned char;
	constexpr Byte    OneByte  = 1;
	constexpr UInt64  OneKByte = 1024 * OneByte;
	constexpr UInt64  OneMByte = 1024 * OneKByte;
	constexpr UInt64  OneGByte = 1024 * OneMByte;
	using EnumMask	= std::uint32_t;
	using EnumBit	= std::uint32_t;
	template<typename T> inline
	UInt64 BytesOf() { return sizeof(T); }
	template<typename T> inline
	UInt64 BitsOf()  { return 8 * BytesOf<T>(); }
	template<Integer T> inline
	Bool IsPowerOfTwo(T _Number) { return (_Number > 0) && ((_Number & (_Number - 1)) == 0); }

	using ID		= UInt32;
	using Token		= std::uint64_t;
	using Address   = void*;
	using ErrorCode = std::int32_t;

	constexpr Bool False   = false;
	constexpr Bool True    = !False;

	/* <<  Containers >>
	    1. Array
	    2. Set
	    3. HashMap
	    4. Segment
	*/

	template<typename T>
	using Array	   = std::vector<T>;

	template<typename T>
	using List	   = std::list<T>;

	template<typename T>
	using Set	   = std::unordered_set<T>;

	template<typename Key, typename Value>
	using HashMap  = std::unordered_map<Key, Value>;

	template<typename T, size_t Length>
	using Segment  = std::array<T, Length>;

	template<typename T1, typename T2>
	using Pair      = std::pair<T1, T2>;

	template<typename T1, typename T2> constexpr
	Bool IsOrderedPair(const Pair<T1, T2>& _Pair) { return _Pair.second >= _Pair.first; }

	template <typename... Args>
	using ResultPackage = std::tuple<Args...>;

	/* <<  Pointers >>
	    1. SharedPtr
	    2. WeakPtr
	    3. UniquePtr
	*/

	template<typename T>
	using SharedPtr   = std::shared_ptr<T>;
	template<typename T, typename... Args>
	inline SharedPtr<T>
	CreateSharedPtr(Args &&...args) { return std::make_shared<T>(std::forward<Args>(args)...); }

	template<typename T>
	using WeakPtr	  = std::weak_ptr<T>;

	template<typename T>
	using UniquePtr   = std::unique_ptr<T>;
	template<typename T, typename... Args>
	inline UniquePtr<T>
	CreateUniquePtr(Args &&...args) { return std::make_unique<T>(std::forward<Args>(args)...); }

	template<typename T>
	using Optional	  = std::optional<T>;

} // namespace VE