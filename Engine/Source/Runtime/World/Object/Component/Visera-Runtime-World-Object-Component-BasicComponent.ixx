module;
#include <Visera.h>
export module Visera.Runtime.World.Object.Component:BasicComponent;

import Visera.Core.Type;
import Visera.Core.Signal;

export namespace VE
{
	class VObject;

	class OCBasicComponent
	{
		friend class VObject; // Object Components are managed by the VObject.
	public:
		auto GetComponentName()				-> StringView { return Name.GetFileName(); }
		auto GetComponentNameWithNumber()   -> String { return Name.GetNameWithNumber(); }
		auto GetOwner() -> WeakPtr<VObject> { return Owner; }
	
	protected:
		virtual void Update() {};
		virtual void Create() = 0;
		virtual void Destroy() = 0;

	public:
		OCBasicComponent() = delete;
		OCBasicComponent(FName _Name, SharedPtr<VObject> _Owner)
			:Name{ _Name }, Owner{ _Owner }
		{ VE_ASSERT(!Name.IsNone() && _Owner != nullptr); }
				
	private:
		FName Name;
		WeakPtr<VObject> Owner;
	};

} // namespace VE