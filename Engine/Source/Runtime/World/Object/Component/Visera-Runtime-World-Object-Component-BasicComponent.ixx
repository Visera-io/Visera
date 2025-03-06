module;
#include <Visera.h>
export module Visera.Runtime.World.Object.Component:BasicComponent;
import Visera.Core.Type;
import Visera.Core.Signal;

export namespace VE
{

	class OCBasicComponent
	{
	public:
		auto GetComponentName()				-> StringView { return Name.GetFileName(); }
		auto GetComponentNameWithNumber()   -> String { return Name.GetNameWithNumber(); }

	protected:
		virtual void Update() {};
		virtual void Create() = 0;
		virtual void Destroy() = 0;

	public:
		OCBasicComponent() = delete;
		OCBasicComponent(FName _Name):Name{ _Name } { VE_ASSERT(!Name.IsNone()); }
		virtual ~OCBasicComponent() = default;
				
	private:
		FName Name;
		//WeakPtr<VObject> Owner;
	};

} // namespace VE