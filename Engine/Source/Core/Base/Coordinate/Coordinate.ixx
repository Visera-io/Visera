module;

export module Visera.Core.Base.Coordinate;
export import :Axis;
export import :World;

namespace VE
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

} // namespace VE