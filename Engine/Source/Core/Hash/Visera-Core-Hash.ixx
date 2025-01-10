module;
#include <Visera.h>

export module Visera.Core.Hash;
import :CityHash;

VISERA_PUBLIC_MODULE
namespace Hash
{
	using UInt128 = uint128;

	/*<<City Hash by Google>>*/
	UInt64  inline CityHash64(const String& _String)	{ return CityHash64(_String.data(), _String.size()); }
	UInt64  inline CityHash64(const String& _String,  UInt64 _Seed) { return CityHash64WithSeed(_String.data(), _String.size(), _Seed); }
	UInt128 inline CityHash128(const String& _String)	{ return CityHash128(_String.data(), _String.size()); }
	UInt128 inline CityHash128(const String& _String,  UInt128 _Seed) { return CityHash128WithSeed(_String.data(), _String.size(), _Seed); }

}
VISERA_MODULE_END