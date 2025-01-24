module;
#include <Visera.h>

export module Visera.Runtime.World;
import Visera.Runtime.World.Atlas;
import Visera.Runtime.World.Object;
import Visera.Runtime.World.Stage;

import Visera.Core.Type;

export namespace VE { namespace Runtime
{

	class World
	{
		VE_MODULE_MANAGER_CLASS(World);
	public:
		VE_API CreateStage(FName _StageName, const String& _StageFile) -> SharedPtr<VStage> { return CreateSharedPtr<VStage>(_StageName, _StageFile); }
		VE_API GetCoordinate() -> const FViseraChart& { return Atlas::Visera; }
	};

} } // namespace VE::Runtime