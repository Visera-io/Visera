module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.World.RTC.Embree;
export import :Enums;
export import :Device;

export namespace VE { namespace Runtime
{
	class RHI;
	
	class FEmbree
	{
		friend class RHI;
	public:
		auto GetDevice() const -> const FEmbreeDevice& { return Device; }
		//

	private:
		FEmbreeDevice Device;

	public:
		FEmbree();
		~FEmbree();
	};

	FEmbree::
	FEmbree()
	{
		
	}

	FEmbree::
	~FEmbree()
	{

	}

} } // namespace Visera::Runtime