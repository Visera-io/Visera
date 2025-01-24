module;
#include <Visera.h>
export module Visera.Runtime.World.Atlas;
export import Visera.Runtime.World.Atlas.Chart;

export namespace VE { namespace Runtime
{

	class Atlas
	{
		VE_MODULE_MANAGER_CLASS(Atlas);
	
		VE_API const Visera = FViseraChart();
	};
	
} } // namespace VE::Runtime