module;
#include <Visera.h>
export module Visera.Core.Math.Hash;
import :CityHash;

export namespace VE
{

	class Hash
	{
		VE_MODULE_MANAGER_CLASS(Hash);
	public:
		using UInt128 = Google::uint128;
		/*<<City Hash by Google>>*/
		VE_API  CityHash64(const String& _String)					-> UInt64	{ return Google::CityHash64(_String.data(), _String.size()); }
		VE_API  CityHash64(const String& _String,  UInt64 _Seed)	-> UInt64	{ return Google::CityHash64WithSeed(_String.data(), _String.size(), _Seed); }
		VE_API	CityHash128(const String& _String)					-> UInt128	{ return Google::CityHash128(_String.data(), _String.size()); }
		VE_API	CityHash128(const String& _String,  UInt128 _Seed)	-> UInt128	{ return Google::CityHash128WithSeed(_String.data(), _String.size(), _Seed); }

	};

} // namespace VE