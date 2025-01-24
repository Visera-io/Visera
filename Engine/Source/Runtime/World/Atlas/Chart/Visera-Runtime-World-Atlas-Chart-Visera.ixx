module;
#include <Visera.h>
export module Visera.Runtime.World.Atlas.Chart:Visera;

import Visera.Core.Math;

export namespace VE { namespace Runtime
{
	/*   [Visera World Coordinate]  * [Standard Vulkan Coordinate]  *
	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	*								*								*
	*        (Y)					*         O---------------(X)	*
	*         |						*         |\					*
	*         |     (Z)				*         | \					*
	*         |     /				*         |  \					*
	*         |    /				*         |   \					*
	*         |   /					*         |    \				*
	*         |  /					*         |     \				*
	*         | /					*         |     (Z)				*
	*         |/					*         |						*
	*         O---------------(X)	*        (Y)					*
	*								*								*
	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	* [Note]														*
	* We transfer "Vulkan Coordinate" to "Visera World Coordinate"	*
	* by enabling VK_KHR_maintenance1 device extension.				*
	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	class FViseraChart
	{
	public:
		union
		{
			Vector3F O{ 0 , 0 , 0 };
			Vector3F Origin;
		};
		union
		{
			Vector3F X{ 1 , 0 , 0 };
			Vector3F Right;
		};
		union
		{
			Vector3F Y{ 0 , 1 , 0 };
			Vector3F Upward;
		};
		union
		{
			Vector3F Z{ 0 , 0 , 1 };
			Vector3F Forward;
		};
	};

} } // namespace VE::Runtime