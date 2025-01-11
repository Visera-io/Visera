module;
#include <Visera.h>

export module Visera.Runtime.World.Stage;
import Visera.Runtime.World.Stage.Scene;

import Visera.Runtime.World.Object;

export namespace VE { namespace Runtime
{

	class VStage : public VObject
	{
	public:
		VStage();
		~VStage();

	private:
		//[TODO]: NamePool
		HashMap<Token, SharedPtr<VScene>> Scenes;
	};

	VStage::
	VStage() : VObject("Hello")
	{

	}

	VStage::
	~VStage()
	{

	}

} } // namespace VE::Runtime