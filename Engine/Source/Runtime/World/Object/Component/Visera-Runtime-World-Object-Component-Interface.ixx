module;
#include <Visera.h>
export module Visera.Runtime.World.Object.Component:Interface;

import Visera.Core.Type;

export namespace VE { namespace Runtime
{
	class VObject;

	class OCComponent
	{
		friend class VObject; // Object Components are managed by the VObject.
	protected:
		virtual void Update() {};
		virtual void Create() = 0;
		virtual void Destroy() = 0;

		OCComponent() = delete;
		OCComponent(FName _Name, SharedPtr<VObject> _Owner)
			:Name{ _Name }, Owner{ _Owner }
		{
			VE_ASSERT(!Name.IsNone() && _Owner != nullptr);
		}

		FName Name;
		WeakPtr<VObject> Owner;
	};

} } // namespace VE::Runtime