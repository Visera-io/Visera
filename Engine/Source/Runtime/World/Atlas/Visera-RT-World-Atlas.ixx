module;
#include <Visera.h>
export module Visera.Runtime.World.Atlas;
export import Visera.Runtime.World.Atlas.Chart;

export namespace VE
{

	class Atlas
	{
		VE_MODULE_MANAGER_CLASS(Atlas);
	public:
	    const static inline auto Visera = FViseraChart();
	};
	
} // namespace VE