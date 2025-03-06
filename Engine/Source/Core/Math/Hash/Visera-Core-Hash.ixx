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
		VE_API  CityHash64(StringView _StringView)					-> UInt64	{ return Google::CityHash64(_StringView.data(), _StringView.size()); }
		VE_API  CityHash64(StringView _StringView, UInt64 _Seed)	-> UInt64	{ return Google::CityHash64WithSeed(_StringView.data(), _StringView.size(), _Seed); }
		VE_API	CityHash64(const char* _String, UInt64 _Length)		-> UInt64	{ return Google::CityHash64(_String, _Length); }
		VE_API	CityHash64(const char* _String, UInt64 _Length, UInt64 _Seed) -> UInt64	{ return Google::CityHash64WithSeed(_String, _Length, _Seed); }
		
		VE_API  CityHash128(StringView _StringView)					-> UInt128	{ return Google::CityHash128(_StringView.data(), _StringView.size()); }
		VE_API  CityHash128(StringView _StringView, UInt128 _Seed)	-> UInt128	{ return Google::CityHash128WithSeed(_StringView.data(), _StringView.size(), _Seed); }
		VE_API	CityHash128(const char* _String, UInt64 _Length)		-> UInt128	{ return Google::CityHash128(_String, _Length); }
		VE_API	CityHash128(const char* _String, UInt64 _Length, UInt128 _Seed) -> UInt128 { return Google::CityHash128WithSeed(_String, _Length, _Seed); }
	};

} // namespace VE