#pragma once
// << Layers >>
#define VISERA_RUNTIME

#define VISERA_YEAR     2024
#define VISERA_AUTHOR   "LJYC (ljyc.me)"

#include "PCH/Visera-STD.h"
#include "PCH/Visera-Types.h"

// << Global Macros >>
#define VISERA_ENGINE_ERROR -1
#define VISERA_APP_ERROR    -2

#define VISERA_PUBLIC_MODULE    export namespace VE {
#define VISERA_PRIVATE_MODULE   namespace VE {
#define VISERA_MODULE_END       }

#define VE_ASSERT(Expression)	assert(Expression);

#define VE_REGISTER_AUTOCAST(IType, OType) constexpr OType AutoCast(IType src) { return static_cast<OType>(src); }

#define VE_OP_BITOR(IType, OType)	constexpr OType operator|(IType lhs, IType rhs) { return static_cast<OType>(static_cast<EnumBit>(lhs) | static_cast<EnumBit>(rhs)); }
#define VE_OP_BITXOR(IType, OType)	constexpr OType operator^(IType lhs, IType rhs) { return static_cast<OType>(static_cast<EnumBit>(lhs) ^ static_cast<EnumBit>(rhs)); }
#define VE_OP_BITAND(IType, OType)	constexpr OType operator&(IType lhs, IType rhs) { return static_cast<OType>(static_cast<EnumBit>(lhs) & static_cast<EnumBit>(rhs)); }

#define VE_ENUM_CLASS(Name, Super)		\
		enum class Name : EnumBit;		\
		VE_REGISTER_AUTOCAST(Name,Super)\
		VE_OP_BITOR(Name, Name)			\
		VE_OP_BITXOR(Name, Name)		\
		VE_OP_BITAND(Name, Name);		\
		enum class Name : EnumBit