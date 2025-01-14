module;
#include <Visera.h>

export module Visera.Runtime.World;
import Visera.Runtime.World.Object;
import Visera.Runtime.World.Stage;

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

	class World
	{
		VE_MODULE_MANAGER_CLASS(World);
	public:
		auto CreateStage(const String& _StageFile) -> SharedPtr<VStage> { return nullptr; }

	private:

	};

} } // namespace VE::Runtime